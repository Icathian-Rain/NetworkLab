#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <QString>
#include<cstring>
#include<iostream>
#include<cstdio>
#include<ctime>
#include<chrono>
#include<vector>

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

#define TFTP_NETASCII 0
#define TFTP_OCTET 1

using namespace std;

std::string getCurrentTime();
extern FILE *fp;

class Record
{
private:
    /* data */
    std::string time;
    std::string type;
    std::string status;
    std::string msg;

public:
    Record(int type,int status,std::string msg);
    explicit Record(std::string);
    ~Record();
    std::string toString();
    void fromString(std::string str);
};


class workThread : public QThread
{
Q_OBJECT
private:
    char serverIP[20];
    int port;
    char fileName[100];
    int op;
    int mode;

    FILE *fp1;

    chrono::high_resolution_clock::time_point startTime;
    chrono::high_resolution_clock::time_point endTime;
    chrono::high_resolution_clock::time_point timePoint;


public:
    workThread();
    ~workThread();
    void set(char *ip, int port, char *fileName, int op, int mode);
    bool Request(char *ip, int port, char *fileName, int op, int mode);
    static void clear(FILE *, SOCKET);
    void startV();
    void stopV();
    void showInsV(int bytes);
    void showAvgV(int bytes);
    void write(Record record);
    void read(std::vector<Record> &records);
    signals: void sendV(QString s);
    void sendM(QString s);

protected:
    void run();

};

#endif // WORKTHREAD_H
