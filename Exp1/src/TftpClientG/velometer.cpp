//
// Created by 22057 on 2022/10/22.
//

#include "velometer.h"

velometer::velometer(/* args */)
= default;

velometer::~velometer()
= default;

void velometer::start()
{
    startTime = chrono::high_resolution_clock::now();
    timePoint = startTime;
}

void velometer::stop()
{
    endTime = chrono::high_resolution_clock::now();
}

void velometer::showInsV(int bytes, QTextBrowser *browser)
{
    auto nowTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> time_span = chrono::duration_cast<chrono::duration<double>>(nowTime - timePoint);
    double time = time_span.count();
    double velo = (double)bytes / time / 1024 * 1000;
    char buff[100];
    sprintf(buff, " %.2f KB/s", velo);
//    browser->setText(buff);
    emit setV(buff);
    timePoint = nowTime;
}

void velometer::showAvgV(int bytes, QTextBrowser *browser)
{
    chrono::duration<double, std::milli> time_span = chrono::duration_cast<chrono::duration<double>>(endTime - startTime);
    double time = time_span.count();
    double velo = (double)bytes / time / 1024 * 1000;
    char buff[100];
    sprintf(buff, " %.2f KB/s", velo);
//    browser->setText(buff);
    emit setV(buff);
}



