#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* 注意！该初始化不能在Main中进行，否则会出现未知问题导致程序无法正常运行！！！ */
    Server *server = new Server();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test(){
    qDebug() << "asdasd";
}
