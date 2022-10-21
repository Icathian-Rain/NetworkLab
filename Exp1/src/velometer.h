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
public:
    velometer(/* args */);
    ~velometer();
    void start();
    void stop();
    void reset();
    void showV(int bytes);
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
}

void velometer::stop()
{
    endTime = chrono::high_resolution_clock::now();
}

void velometer::reset()
{
    startTime = chrono::high_resolution_clock::now();
    endTime = chrono::high_resolution_clock::now();
}

void velometer::showV(int bytes)
{
    chrono::duration<double, std::milli> time_span = chrono::duration_cast<chrono::duration<double>>(endTime - startTime);
    double time = time_span.count();
    double velo = (double)bytes / time / 1024 * 1000;
    printf("Speed: %.2f KB/s time: %.2f ms\r", velo, time);
}




