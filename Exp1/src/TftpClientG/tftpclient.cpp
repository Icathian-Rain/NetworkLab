#include "tftpclient.h"


/// @brief 初始化tftp客户端
/// @param ip 目的服务器IP
/// @param port 目的服务器端口
tftp::TftpClient::TftpClient(const char *ip, int port = TFTP_PORT)
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    strcpy(this->serverIP, ip);
    this->port = port;
    velo = new velometer();
}

tftp::TftpClient::~TftpClient()
{
    WSACleanup();
}

/// @brief 清理函数
/// @param fp
/// @param sock
void tftp::TftpClient::clear(FILE *fp, SOCKET sock)
{
    fclose(fp);
    closesocket(sock);
}


void tftp::TftpClient::setServerIP(const char *ip) {
    strcpy(serverIP, ip);
}

void tftp::TftpClient::setPort(int port) {
    this->port = port;
}


/// @brief TFTP发送请求包
/// @param op 1读文件  2写文件
/// @param fileName 文件名
/// @param mode 0 octet 字节流  1 netascii ascii码
/// @return 是否成功
int tftp::TftpClient::Request(int op,char *fileName, int mode, QTextBrowser *browser)
{
    const char *TransferMode[2] = {"netascii", "octet"};
    // 初始化目的地址
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->port);
    addr.sin_addr.S_un.S_addr = inet_addr(this->serverIP);

    // 初始化客户端套接字
    SOCKET clientSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSock == -1)
    {
        logger.write(Record(0, 1, "socket error"));
        return -1;
    }

    // 请求信息
    char reqBuff[1024];
    int reqLen = 0;
    memset(reqBuff, 0, sizeof(reqBuff));

    // 接收信息
    unsigned char recvBuff[1024];
    int recvLen = 0;
    memset(recvBuff, 0, 1024);

    // 发送请求信息 0 + op + filename + 0 + mode + 0
    sprintf(reqBuff, "%c%c%s%c%s", 0, op, fileName, 0, TransferMode[mode]);
    reqLen = strlen(fileName) + strlen(TransferMode[mode]) + 4;

    // 超时处理
    struct timeval tv;
    fd_set readfds;
    int errNum = 0;

    // 打开文件
    FILE *fp;
    unsigned short ackNum = 0;
    if (op == TFTP_OPCODE_RRQ && mode == TFTP_NETASCII)
        fp = fopen(fileName, "w");
    else if (op == TFTP_OPCODE_RRQ && mode == TFTP_OCTET)
        fp = fopen(fileName, "wb");
    else if (op == TFTP_OPCODE_WRQ && mode == TFTP_NETASCII)
        fp = fopen(fileName, "r");
    else if (op == TFTP_OPCODE_WRQ && mode == TFTP_OCTET)
        fp = fopen(fileName, "rb");

    if (fp == nullptr)
    {
        logger.write(Record(0, 1, "open file error"));
        return -1;
    }

    if( op == TFTP_OPCODE_RRQ)
    logger.write(Record(0, 0, "start sendto"));
    else if (op == TFTP_OPCODE_WRQ)
    logger.write(Record(0, 0, "start receive"));

    // 计时器

    int bytes = 0;
    int totalBytes = 0;
    velo->start();
    int i;
    for(i = 0; ; i++)
    {
        // 测速器输出速率, 每1000个包输出一次
        if(i % 1000 == 0)
        {
            velo->showInsV(bytes, browser);
            bytes = 0;
        }
        // 发送请求
        int  err = sendto(clientSock, reqBuff, reqLen, 0, (struct sockaddr *)&addr, sizeof(addr));
        if (err == -1)
        {
            logger.write(Record(0, 1, "sendto error"));
            return -1;
        }
        // 超时处理
        FD_ZERO(&readfds);
        FD_SET(clientSock, &readfds);
        tv.tv_sec = TFTP_TIMEOUT;
        tv.tv_usec = 0;
        select(clientSock + 1, &readfds, NULL, NULL, &tv);

        if (FD_ISSET(clientSock, &readfds))
        {
            // 接收数据
            struct sockaddr_in from;
            int fromlen = sizeof(from);
            recvLen = recvfrom(clientSock, (char *)recvBuff, 1024, 0, (struct sockaddr *)&from, &fromlen);
            if (recvLen > 0)
            {
                errNum = 0;
                // 更改目的地址
                memcpy((struct sockaddr *)&addr, (struct sockaddr *)&from, sizeof(fromlen));
                // 获取分组号
                int blockNum = (recvBuff[2] << 8) + recvBuff[3];
                // 获取操作码
                int opcode = (recvBuff[0] << 8) + recvBuff[1];
                // 判断操作类型
                if (op == TFTP_OPCODE_RRQ)
                // 若为读取操作
                {
                    if (opcode == TFTP_OPCODE_DATA)
                    {
                        if(blockNum == (ackNum + 1))
                        {
                            ackNum++;
                            sprintf(reqBuff, "%c%c%c%c", 0, TFTP_OPCODE_ACK, recvBuff[2], recvBuff[3]);
                            fwrite(recvBuff + 4, 1, recvLen - 4, fp);
                            bytes += recvLen - 4;
                            totalBytes += recvLen - 4;
                            reqLen = 4;
                            if (recvLen < 516)
                            {
                                sendto(clientSock, reqBuff, reqLen, 0, (struct sockaddr *)&addr, sizeof(addr));
                                break;
                            }
                        }
                    }
                    else if (opcode == TFTP_OPCODE_ERROR)
                    {
                        char errMsg[100];
                        sprintf(errMsg, "error code: %d", recvBuff[3]);
                        logger.write(Record(3, 1, errMsg));
                        clear(fp, clientSock);
                        return -1;
                    }
                    else
                    {
                        logger.write(Record(3, 1, "unknown opcode"));
                        clear(fp, clientSock);
                        return -1;
                    }
                }
                if (op == TFTP_OPCODE_WRQ)
                {
                    if (opcode == TFTP_OPCODE_ACK)
                    {
                        if(blockNum == ackNum)
                        {
                            ackNum++;
                            sprintf(reqBuff, "%c%c%c%c", 0, TFTP_OPCODE_DATA, ackNum >> 8, ackNum & 0xff);
                            reqLen = fread(reqBuff + 4, 1, TFTP_DATA_SIZE, fp);
                            bytes += reqLen;
                            totalBytes += reqLen;
                            if (reqLen == 0)
                            {
                                break;
                            }
                            reqLen += 4;
                        }
                    }
                    else if (opcode == TFTP_OPCODE_ERROR)
                    {
                        char errMsg[100];
                        sprintf(errMsg, "error code: %d", recvBuff[3]);
                        logger.write(Record(3, 1, errMsg));
                        clear(fp, clientSock);
                        return -1;
                    }
                    else
                    {
                        logger.write(Record(3, 1, "unknown opcode"));
                        clear(fp, clientSock);
                        return -1;
                    }
                }
            }
        }
        else
        {
            logger.write(Record(3, 1, "timeout"));
            errNum++;
            if (errNum >= TFTP_RETRY)
            {
                logger.write(Record(2, 1, "retry too many times"));
                clear(fp, clientSock);
                return -1;
            }
        }
    }
    velo->stop();
    velo->showAvgV(totalBytes, browser);
    logger.write(Record(1, 0, "transfer complete, total blocks: " + to_string(i) + ", total bytes: " + to_string(totalBytes)));
    clear(fp, clientSock);
    return 0;
}




