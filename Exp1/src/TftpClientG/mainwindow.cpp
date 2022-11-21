#include "mainwindow.h"

#include "./ui_mainwindow.h"

FILE  *fp = fopen("log.txt", "w");
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    char ip[20] = "127.0.0.1";
    ui->setupUi(this);
    ui->directoryPath->setPlaceholderText("./");
    ui->serverIP->setPlainText("127.0.0.1");
    ui->serverPort->setValue(69);
    ui->fileName->setPlainText("test.txt");
}

MainWindow::~MainWindow()
{
    fclose(fp);
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, "请选择客户端目录", "./");
    SetCurrentDirectoryA(filePath.toStdString().c_str());
    ui->directoryPath->setPlainText(filePath);
}


void MainWindow::on_downLoad_clicked()
{

}


void MainWindow::on_upLoad_clicked()
{
}


void MainWindow::on_pushButton_2_clicked()
{
    // 新建工作线程
    auto thread = new workThread();
    // 获取参数
    char ip[20], fileName[100];
    int port, op, mode;
    strcpy(ip, ui->serverIP->toPlainText().toStdString().c_str());
    port = ui->serverPort->value();
    op = ui->upLoad->isChecked() ? TFTP_OPCODE_WRQ : TFTP_OPCODE_RRQ;
    mode = ui->netasciiMode->isChecked() ? TFTP_NETASCII : TFTP_OCTET;
    strcpy(fileName, ui->fileName->toPlainText().toStdString().c_str());
    // 连接信号槽
    connect(thread, SIGNAL(sendV(QString)), this, SLOT(on_sendV(QString)));
    connect(thread, SIGNAL(sendM(QString)), this, SLOT(on_sendM(QString)));
    // 配置线程
    thread->set(ip, port, fileName, op, mode);
    // 启动线程
    thread->start();
}

/// @brief 接收速率信息
/// @param s 
void MainWindow::on_sendV(QString s)
{
    ui->speedtext->setPlainText(s);
}

/// @brief 接收日志信息
/// @param s 
void MainWindow::on_sendM(QString s)
{
    ui->msgtext->append(s);
}

