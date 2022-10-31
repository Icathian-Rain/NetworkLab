#include<iostream>
#include<string>
#include<time.h>
#include<cstdio>
#include<vector>

const char *recordType[] = {"用户操作", "传输成功", "传输失败", "超时重传"};

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


class Log
{
private:
    /* data */
    FILE* fp;
public:
    Log(/* args */);
    Log(std::string);
    ~Log();
    void write(Record record);
    void read(std::vector<Record> &records);
};

Log::Log(/* args */)
{
    fp = fopen("log.txt", "w");
}

Log::Log(std::string path)
{
    fp = fopen(path.c_str(), "w");
}

Log::~Log()
{
    fclose(fp);
}

void Log::write(Record record)
{
    std::string msg = record.toString() + "\n";
    fwrite(msg.c_str(), sizeof(char), msg.length(), fp);
    std::cout << msg << std::endl;
}

void Log::read(std::vector<Record> &records)
{
    char tmp[1024];
    while (fgets(tmp, 1024, fp) != NULL)
    {
        Record record(tmp);
        records.push_back(record);
    }
}


