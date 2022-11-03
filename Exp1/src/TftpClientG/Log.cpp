//
// Created by 22057 on 2022/10/22.
//

#include "Log.h"

#include <utility>
std::string getCurrentTime()
{
    time_t nowtime;
    struct tm t;
    nowtime = time(NULL);
    // localtime_s(&t, &nowtime);
    t = *localtime(&nowtime);
    char tmp[64];
    strftime(tmp,sizeof(tmp),"%Y-%m-%d %H:%M:%S",&t);
    return tmp;
}


/// @brief 构造函数
/// @param type 记录类型 0用户操作 1传输成功 2传输失败 3超时重传
/// @param status 记录状态 0成功 1失败
/// @param msg 记录信息 传输文件名或者错误信息
Record::Record(int type,int status, std::string msg)
{
    const char *recordType[] = {"用户操作", "传输成功", "传输失败", "超时重传"};
    const char *recordStatus[] = {"成功", "失败"};
    this->time = getCurrentTime();
    this->type = recordType[type];
    this->status = recordStatus[status];
    this->msg = std::move(msg);
}

/// @brief 构造函数2
/// @param str 模板字符串
Record::Record(std::string str)
{
    fromString(std::move(str));
}

Record::~Record()
= default;

std::string Record::toString()
{
    return this->time + " "  + this->type + " " + this->status + " " + this->msg;
}

void Record::fromString(std::string str)
{
    int pos = str.find(" ");
    this->time = str.substr(0, pos);
    str = str.substr(pos + 1);
    pos = str.find(" ");
    this->status = str.substr(0, pos);
    str = str.substr(pos + 1);
    pos = str.find(" ");
    this->type = str.substr(0, pos);
    str = str.substr(pos + 1);
    this->msg = str;
}

Log::Log(/* args */)
{
    fp1 = fopen("log.txt", "w");
}

Log::Log(const std::string& path)
{
    fp1 = fopen(path.c_str(), "w");
}

Log::~Log()
{
    fclose(fp1);
}

void Log::write(Record record)
{
    std::string msg = record.toString() + "\n";
    std::cout << msg << std::endl;
    fwrite(msg.c_str(), sizeof(char), msg.length(), fp1);
}

void Log::read(std::vector<Record> &records)
{
    char tmp[1024];
    while (fgets(tmp, 1024, fp1) != nullptr)
    {
        Record record(tmp);
        records.push_back(record);
    }
}