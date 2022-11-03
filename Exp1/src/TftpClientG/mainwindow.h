#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include "workthread.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_downLoad_clicked();
    void on_upLoad_clicked();
    void on_pushButton_2_clicked();
    void on_sendV(QString s);
    void on_sendM(QString s);

private:
    Ui::MainWindow *ui;
    workThread *thread;
};
#endif // MAINWINDOW_H
