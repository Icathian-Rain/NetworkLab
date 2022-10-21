#include<iostream>
#include"tftp.h"


int main()
{
    SetCurrentDirectoryA("..\\test\\client");
    char fileName[100] = "test.jpg";
    tftp::tftpClient client("127.0.0.1");
    client.read(fileName, TFTP_OCTET);
    return 0;
}