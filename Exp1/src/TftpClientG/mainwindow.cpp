#include "mainwindow.h"

#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    char ip[20] = "127.0.0.1";
    this->thread = new workThread();
    ui->setupUi(this);
    ui->directoryPath->setPlaceholderText("./");
    ui->serverIP->setPlainText("127.0.0.1");
    ui->serverPort->setValue(69);
    ui->fileName->setPlainText("test.txt");
    connect(this->thread, SIGNAL(sendV(QString)), this, SLOT(on_sendV(QString)));
    connect(this->thread, SIGNAL(sendM(QString)), this, SLOT(on_sendM(QString)));
}

MainWindow::~MainWindow()
{
    delete this->thread;
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
    char ip[20], fileName[100];
    int port, op, mode;
    strcpy(ip, ui->serverIP->toPlainText().toStdString().c_str());
    port = ui->serverPort->value();
    op = ui->upLoad->isChecked() ? TFTP_OPCODE_WRQ : TFTP_OPCODE_RRQ;
    mode = ui->netasciiMode->isChecked() ? TFTP_NETASCII : TFTP_OCTET;
    strcpy(fileName, ui->fileName->toPlainText().toStdString().c_str());
    thread->set(ip, port, fileName, op, mode);
    thread->start();
}

void MainWindow::on_sendV(QString s)
{
    ui->speedtext->setPlainText(s);
}

void MainWindow::on_sendM(QString s)
{
    ui->msgtext->append(s);
}

