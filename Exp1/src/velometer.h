#include<iostream>
#include<time.h>
#include<chrono>

using namespace std;

class velometer
{
private:
    /* data */
    chrono::high_resolution_clock::time_point startTime;
    chrono::high_resolution_clock::time_point endTime;
    chrono::high_resolution_clock::time_point timePoint;
public:
    velometer(/* args */);
    ~velometer();
    void start();
    void stop();
    void showInsV(int bytes);
    void showAvgV(int bytes);
};

velometer::velometer(/* args */)
{

}

velometer::~velometer()
{
}

void velometer::start()
{
    startTime = chrono::high_resolution_clock::now();
    timePoint = startTime;
}

void velometer::stop()
{
    endTime = chrono::high_resolution_clock::now();
}


void velometer::showInsV(int bytes)
{
    auto nowTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> time_span = chrono::duration_cast<chrono::duration<double>>(nowTime - timePoint);
    double time = time_span.count();
    double velo = (double)bytes / time / 1024 * 1000;
    printf("now Speed: %.2f KB/s\r", velo);
    timePoint = nowTime;
}

void velometer::showAvgV(int bytes)
{
        chrono::duration<double, std::milli> time_span = chrono::duration_cast<chrono::duration<double>>(endTime - startTime);
    double time = time_span.count();
    double velo = (double)bytes / time / 1024 * 1000;
    printf("\nAvg Speed: %.2f KB/s\n", velo);
}




