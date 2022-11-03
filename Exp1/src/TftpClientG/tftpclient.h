#ifndef TFTPCLIENT_H
#define TFTPCLIENT_H

#include <cstdio>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "velometer.h"
#include "Log.h"





// tftp命名空间
namespace tftp
{
    class TftpClient
    {
    private:

        char serverIP[20];  // 服务器IP
        int port;        // 端口号
        Log logger;      // 日志

    public:
        TftpClient(const char *ip, int port);
        ~TftpClient();
        velometer *velo;
        void setServerIP(const char *ip);
        void setPort(int port);
        int Request(int op,char *fileName, int mode, QTextBrowser *textBrowser);
        void clear(FILE *, SOCKET);
    };




} // namespace tftp


#endif // TFTPCLIENT_H
