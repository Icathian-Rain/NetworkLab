//
// Created by 22057 on 2022/10/22.
//

#ifndef TFTPCLIENTG_VELOMETER_H
#define TFTPCLIENTG_VELOMETER_H
#include<iostream>
#include<QObject>
#include<cstring>
#include<cstdio>
#include<QtWidgets/QTextBrowser>
#include<ctime>
#include<chrono>

using namespace std;

class velometer:public QObject
{
    Q_OBJECT
private:
    /* data */
    chrono::high_resolution_clock::time_point startTime;
    chrono::high_resolution_clock::time_point endTime;
    chrono::high_resolution_clock::time_point timePoint;
public:
    velometer(/* args */);
    ~velometer();    void start();
    void stop();
    void showInsV(int bytes, QTextBrowser *browser);
    void showAvgV(int bytes, QTextBrowser *browser);
    signals: void setV(char *v);
};


#endif //TFTPCLIENTG_VELOMETER_H
