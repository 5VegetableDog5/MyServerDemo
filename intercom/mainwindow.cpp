#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    labelInit();

    /* 注意！该初始化不能在Main中进行，否则会出现未知问题导致程序无法正常运行！！！ */
    this->server = new Server();

    QWidget  *onlineScrollAreaContent = new QWidget ();
    QWidget  *callingScrollAreaContent = new QWidget ();

    ui->onlineScrollArea->setWidgetResizable(true);//窗口自适应
    ui->onlineScrollArea->setWidget(onlineScrollAreaContent);//设置滚动窗口的QWidget

    ui->callingScrollArea->setWidgetResizable(true);
    ui->callingScrollArea->setWidget(callingScrollAreaContent);//同上

    this->onlineVBoxLayout = new QVBoxLayout(onlineScrollAreaContent);//创建垂直布局
    this->callingVBoxLayout = new QVBoxLayout(callingScrollAreaContent);

    connect(server,&Server::newOnlineClient,this,&MainWindow::addNewOnlineClient);
    connect(server,&Server::upgradeClientStatus,this,&MainWindow::upgradeOnlineClient);
    connect(server,&Server::offLineSingal,this,&MainWindow::offLineClient);

    connect(server,&Server::newCalling,this,&MainWindow::addNewCalling);
    connect(server,&Server::deleteCalling,this,&MainWindow::deleteCalling);

    connect(ui->pushButton_Online,&QPushButton::clicked,this,&MainWindow::showOnlineClients);
    connect(ui->pushButton_Calling,&QPushButton::clicked,this,&MainWindow::showCallingClients);
}

MainWindow::~MainWindow()
{
    server->deleteLater();
    onlineVBoxLayout->deleteLater();
    callingVBoxLayout->deleteLater();

    delete ui;
}

void MainWindow::labelInit(){
    ui->label_ip->setStyleSheet("border: 1px solid black;");
    ui->label_status->setStyleSheet("border: 1px solid black;");
    ui->label_dialIP->setStyleSheet("border: 1px solid black;");
    ui->label_receiverIP->setStyleSheet("border: 1px solid black;");
    ui->label_CallingStatus->setStyleSheet("border: 1px solid black;");
    ui->label_3->setStyleSheet("border: 1px solid black;");
    ui->label_4->setStyleSheet("border: 1px solid black;");
    ui->label_CallingTime->setStyleSheet("border: 1px solid black;");
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

    ui->label_clientCount->setText("当前在线用户数:"+QString::number(uiOnlineClient.count()));

}

void MainWindow::upgradeOnlineClient(const QString ipAddr,const short newStatus){
    for(int i = 0; i<uiOnlineClient.count(); i++){
        if(ipAddr == uiOnlineClient[i]->getIPAddr()){
            uiOnlineClient[i]->updataStatus(newStatus);
            //qDebug()<<"update status"<<ipAddr<<newStatus;
        }
    }
    onlineVBoxLayout->update();
}

void MainWindow::offLineClient(const QString ipAddr){
    if(ipAddr == nullptr || ipAddr.isEmpty()){
        qDebug()<<"MainWindow 82行 offLineClient ipAddr is null ";
        return;
    }
    for(int i = 0; i<uiOnlineClient.count(); i++){
        if(ipAddr == uiOnlineClient[i]->getIPAddr()){
            int index = onlineVBoxLayout->indexOf(uiOnlineClient[i]);
            if(index!=-1){
                onlineVBoxLayout->removeWidget(uiOnlineClient[i]);//在ui界面中删除该客户端
                QLayoutItem* item = onlineVBoxLayout->itemAt(index);
                if (item)
                {
                    QWidget* widget = item->widget();
                    if (widget)
                    {
                        onlineVBoxLayout->removeWidget(widget);//删除分割线
                        delete widget; // 手动释放或删除 QWidget 对象
                    }
                }
            }


            OnlineClientUIItem *item = uiOnlineClient[i];//暂时保存该item，等会要释放掉它
            uiOnlineClient.removeAt(i);//在线用户表中删除该用户

            item->deleteLater();//释放内存
            //qDebug()<<"offLine";
            onlineVBoxLayout->update();
            ui->label_clientCount->setText("当前在线用户数:"+QString::number(uiOnlineClient.count()));
            break;
        }
    }

}

void MainWindow::addNewCalling(ClientSocketItem *dialer,ClientSocketItem *receiver){
    CallingClientsUIItem *item = new CallingClientsUIItem(nullptr,dialer,receiver);
    callingVBoxLayout->addWidget(item);

    //添加一条横线
    callingVBoxLayout->addWidget(new QFrame);
    QFrame* separator = qobject_cast<QFrame*>(onlineVBoxLayout->itemAt(onlineVBoxLayout->count() - 1)->widget());
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    uiCallingItems << item;

    ui->label_Calling->setText("通话概况 "+QString::number(uiCallingItems.count()));
}

/*
 * 注意：传进来的参数既可以是拨号者也可以是接听者，不要被变量名误导（懒得改了）
*/
void MainWindow::deleteCalling(ClientSocketItem *dialer){
    for(int i = 0; i<uiCallingItems.count(); i++){
        if(dialer == uiCallingItems[i]->getDialer() || dialer == uiCallingItems[i]->getReceiver()){
            int index = callingVBoxLayout->indexOf(uiCallingItems[i]);

            if(index!=-1){
                callingVBoxLayout->removeWidget(uiCallingItems[i]);//在ui界面中删除该客户端
                QLayoutItem* item = callingVBoxLayout->itemAt(index);
                if (item)
                {
                    QWidget* widget = item->widget();
                    if (widget)
                    {
                        callingVBoxLayout->removeWidget(widget);//删除分割线
                        delete widget; // 手动释放或删除 QWidget 对象
                    }
                }
            }


            CallingClientsUIItem *item = uiCallingItems[i];//暂时保存该item，等会要释放掉它
            uiCallingItems.removeAt(i);//在线用户表中删除该用户

            item->deleteLater();//释放内存
            callingVBoxLayout->update();
            ui->label_Calling->setText("通话概况 "+QString::number(uiCallingItems.count()));
            break;
        }
    }
    //qDebug()<<"error deleteCallingItem!";
}

void MainWindow::pageChanged(int page){
    ui->stackedWidget->setCurrentIndex(page%ui->stackedWidget->count());
}

void MainWindow::showOnlineClients(){
    pageChanged(0);
}
void MainWindow::showCallingClients(){
    pageChanged(1);
}

