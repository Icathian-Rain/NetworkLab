#include<iostream>
#include"tftp.h"


int main()
{
    SetCurrentDirectoryA("..\\test\\client");
    char fileName1[100] = "test.jpg";
    char fileName2[100] = "test.rar";
    tftp::tftpClient client("127.0.0.1");
    // client.read(fileName1, TFTP_OCTET);
    client.write(fileName2, TFTP_OCTET);
    return 0;
}