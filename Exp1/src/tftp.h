#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

// tftp基础设置
#define TFTP_PORT 69
#define TFTP_DATA_SIZE 512
#define TFTP_TIMEOUT 3
#define TFTP_RETRY 3

// tftp操作码
#define TFTP_OPCODE_RRQ 1
#define TFTP_OPCODE_WRQ 2
#define TFTP_OPCODE_DATA 3
#define TFTP_OPCODE_ACK 4
#define TFTP_OPCODE_ERROR 5

// tftp数据流
const char *Transfermode[2] = {"netascii", "octet"};
#define TFTP_NETASCII 0
#define TFTP_OCTET 1


// tftp命名空间
namespace tftp
{
    
    class tftpClient
    {
    private:
        int Request(int op, char *fileName, int mode);
        char serverIP[20];  // 服务器IP
        int port;        // 端口号

    public:
        tftpClient(const char *ip, int port);
        ~tftpClient();
        int read(char *fileName, int mode);
        int write(char *fileName, int mode);
        void clear(FILE *, SOCKET);
    };

    /// @brief 初始化tftp客户端
    /// @param ip 目的服务器IP
    /// @param port 目的服务器端口
    tftpClient::tftpClient(const char *ip, int port = TFTP_PORT)
    {
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
        strcpy(this->serverIP, ip);
        this->port = port;
    }


    tftpClient::~tftpClient()
    {
    }

    /// @brief 清理函数
    /// @param fp 
    /// @param sock 
    void tftpClient::clear(FILE *fp, SOCKET sock)
    {
        fclose(fp);
        closesocket(sock);
        WSACleanup();
    }

    /// @brief TFTP发送请求包
    /// @param op 1读文件  2写文件
    /// @param fileName 文件名
    /// @param mode 0 octet 字节流  1 netascii ascii码
    /// @return 是否成功
    int tftpClient::Request(int op, char *fileName, int mode)
    {
        // 初始化目的地址
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(this->port);
        addr.sin_addr.S_un.S_addr = inet_addr(this->serverIP);

        // 初始化客户端套接字
        SOCKET clientSock = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSock == -1)
        {
            printf("socket is error\n");
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
        sprintf(reqBuff, "%c%c%s%c%s", 0, op, fileName, 0, Transfermode[mode]);
        reqLen = strlen(fileName) + strlen(Transfermode[mode]) + 4;

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

        while (1)
        {
            // 发送请求
            sendto(clientSock, reqBuff, reqLen, 0, (struct sockaddr *)&addr, sizeof(addr));
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
                    // 判断操作类型
                    if (op == TFTP_OPCODE_RRQ)
                    // 若为读取操作
                    {
                        if (recvBuff[1] == TFTP_OPCODE_DATA)
                        {
                            if(blockNum == (ackNum + 1))
                            {
                                ackNum++;
                                sprintf(reqBuff, "%c%c%c%c", 0, TFTP_OPCODE_ACK, recvBuff[2], recvBuff[3]);
                                fwrite(recvBuff + 4, 1, recvLen - 4, fp);
                                reqLen = 4;
                                if (recvLen < 516)
                                {
                                    sendto(clientSock, reqBuff, reqLen, 0, (struct sockaddr *)&addr, sizeof(addr));
                                    break;
                                }
                            }
                        }
                        else if (recvBuff[1] == 5)
                        {
                            printf("error code is %d\n", recvBuff[3]);
                            clear(fp, clientSock);
                            return -1;
                        }
                        else
                        {
                            clear(fp, clientSock);
                            return -1;
                        }
                    }
                    if (op == TFTP_OPCODE_WRQ)
                    {
                        if (recvBuff[1] == TFTP_OPCODE_ACK)
                        {
                            if(blockNum == ackNum)
                            {
                                ackNum++;
                                sprintf(reqBuff, "%c%c%c%c", 0, TFTP_OPCODE_DATA, ackNum >> 8, ackNum & 0xff);
                                reqLen = fread(reqBuff + 4, 1, TFTP_DATA_SIZE, fp);
                                if (reqLen == 0)
                                {
                                    break;
                                }
                                reqLen += 4;
                            }
                        }
                        else if (recvBuff[1] == 5)
                        {
                            printf("error code is %d\n", recvBuff[3]);
                            clear(fp, clientSock);
                            return -1;
                        }
                        else
                        {
                            clear(fp, clientSock);
                            return -1;
                        }
                    }
                    
                }
            }
            else
            {
                printf("time out\n");
                errNum++;
                if (errNum >= TFTP_RETRY)
                {
                    printf("retry times > 5, exit\n");
                    clear(fp, clientSock);
                    return -1;
                }
            }
        }
        clear(fp, clientSock);
        return 0;
    }

    /// @brief 读取文件
    /// @param fileName 文件名
    /// @param mode 文件传输模式 TFTP_NETASCII or TFTP_OCTET
    /// @return 状态
    int tftpClient::read(char *fileName, int mode)
    {
        return Request(TFTP_OPCODE_RRQ, fileName, mode);
    }

    /// @brief 写入文件
    /// @param fileName 文件名
    /// @param mode 文件传输模式 TFTP_NETASCII or TFTP_OCTET
    /// @return 状态
    int tftpClient::write(char *fileName, int mode)
    {
        return Request(TFTP_OPCODE_WRQ, fileName, mode);
    }
} // namespace tftp
