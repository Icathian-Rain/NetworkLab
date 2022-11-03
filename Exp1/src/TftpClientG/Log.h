//
// Created by 22057 on 2022/10/22.
//

#ifndef TFTPCLIENTG_LOG_H
#define TFTPCLIENTG_LOG_H


#include<iostream>
#include<string>
#include<ctime>
#include<cstdio>
#include<vector>




std::string getCurrentTime();

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

class Log
{
private:
    /* data */
    FILE* fp1;
public:
    Log(/* args */);
    Log(const std::string&);
    ~Log();
    void write(Record record);
    void read(std::vector<Record> &records);
};

#endif //TFTPCLIENTG_LOG_H
