#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* 注意！该初始化不能在Main中进行，否则会出现未知问题导致程序无法正常运行！！！ */
    this->server = new Server();

    QWidget  *scrollAreaContent = new QWidget ();
    ui->onlineScrollArea->setWidgetResizable(true);//窗口自适应
    ui->onlineScrollArea->setWidget(scrollAreaContent);//设置滚动窗口的QWidget

    this->onlineVBoxLayout = new QVBoxLayout(scrollAreaContent);//创建垂直布局


    ui->label_ip->setStyleSheet("border: 1px solid black;");
    ui->label_status->setStyleSheet("border: 1px solid black;");


    connect(server,&Server::newOnlineClient,this,&MainWindow::addNewOnlineClient);
    connect(server,&Server::upgradeClientStatus,this,&MainWindow::upgradeOnlineClient);
    connect(server,&Server::offLineSingal,this,&MainWindow::offLineClient);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test(){
    qDebug() << "asdasd";
}

void MainWindow::addNewOnlineClient(const QString ipAddr,const short status){
    OnlineClientUIItem *item = new OnlineClientUIItem(nullptr,ipAddr,status);
    onlineVBoxLayout->addWidget(item);

    //添加一条横线
    onlineVBoxLayout->addWidget(new QFrame);
    QFrame* separator = qobject_cast<QFrame*>(onlineVBoxLayout->itemAt(onlineVBoxLayout->count() - 1)->widget());
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    uiOnlineClient << item;

}

void MainWindow::upgradeOnlineClient(const QString ipAddr,const short newStatus){
    for(int i = 0; i<uiOnlineClient.count(); i++){
        if(ipAddr == uiOnlineClient[i]->getIPAddr()){
            uiOnlineClient[i]->updataStatus(newStatus);
            //qDebug()<<"upgrade status"<<ipAddr<<newStatus;
        }
    }
    onlineVBoxLayout->update();
}

void MainWindow::offLineClient(const QString ipAddr){
    for(int i = 0; i<uiOnlineClient.count(); i++){
        if(ipAddr == uiOnlineClient[i]->getIPAddr()){
            onlineVBoxLayout->removeWidget(uiOnlineClient[i]);//在ui界面中删除该客户端
            OnlineClientUIItem *item = uiOnlineClient[i];//暂时保存该item，等会要释放掉它
            uiOnlineClient.removeAt(i);//在线用户表中删除该用户

            item->deleteLater();//释放内存
            //qDebug()<<"offLine";
            onlineVBoxLayout->update();
            break;
        }
    }

}
