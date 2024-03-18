#include "callingclientsuiitem.h"
#include "ui_callingclientsuiitem.h"

CallingClientsUIItem::CallingClientsUIItem(QWidget *parent,ClientSocketItem *dialer,ClientSocketItem *receiver):
    QWidget(parent),
    ui(new Ui::CallingClientsUIItem)
{
    ui->setupUi(this);
    this->dialer = dialer;
    this->receiver = receiver;

    //UI初始化
#if MONITOR
    this->ui->button_Play->setText("监听");
    this->ui->button_Play->setStyleSheet("background-color: green;");
#endif

    callingTimeInit();//拨号时长Timer初始化
    timerInit();//拨号定时器初始化

    ui->label_Dial->setText(dialer->getSocket()->peerAddress().toString().replace("::ffff:",""));
    ui->label_Receive->setText(receiver->getSocket()->peerAddress().toString().replace("::ffff:",""));

    connect(ui->button_Stop,&QPushButton::clicked,dialer,&ClientSocketItem::hangUPTheCall);
    connect(receiver,&ClientSocketItem::callingStatusChange,this,&CallingClientsUIItem::callingStatusChange);

#if MONITOR
    connect(ui->button_Play,&QPushButton::clicked,this,&CallingClientsUIItem::invertMonitor);
#endif
}

ClientSocketItem* CallingClientsUIItem::getDialer(){
    return this->dialer;
}

ClientSocketItem* CallingClientsUIItem::getReceiver(){
    return this->receiver;
}

void CallingClientsUIItem::callingTimeInit(){
    callingTime = new QTime(0,0,0,0);

    ui->label_CallingTime->setText(callingTime->toString());
}

void CallingClientsUIItem::timerInit(){
    timer = new QTimer(this);
    // 定时器初始化
    // 设置定时器的时间间隔为1秒
    timer->setInterval(1000);
    // 连接定时器的timeout()信号到自定义的槽函数
    connect(timer, &QTimer::timeout, this, &CallingClientsUIItem::timerTimeOut);
}

void CallingClientsUIItem::timerStart(){
    timer->start();
}
void CallingClientsUIItem::timerStop(){
    timer->stop();
}
void CallingClientsUIItem::timerTimeOut(){
    //qDebug()<<"asd"<<callingTime->toString();
    *callingTime = callingTime->addSecs(1);
    ui->label_CallingTime->setText(callingTime->toString());
}

/*
 * 函数功能：用于改变UI界面的通话状态
 * 参数：status 可以是如下几种可能
 *  1. 0 : 未拨通
 *  2. 1 : 拨号中
 *  3. 2 : 已接通
 *
*/
void CallingClientsUIItem::callingStatusChange(int status){
    switch(status){
        case 0:ui->label_CallingStatus->setText("未拨通");break;
        case 1:ui->label_CallingStatus->setText("拨号中");break;
        case 2:ui->label_CallingStatus->setText("已接通");timerStart();break;
        default:ui->label_CallingStatus->setText("未知错误!");
    }
}

/*
*@说明：根据 @STATUS 来设置当前通话是否被监听
*/
void CallingClientsUIItem::setMonitor(bool STATUS){

    closeMONITOR();
    this->monitor = STATUS;
    if(STATUS){
        this->getDialer()->setMonitor(STATUS);
        this->getReceiver()->setMonitor(STATUS);
        this->ui->button_Play->setStyleSheet("background-color: red;");
#if DEBUG
        printfLog(this->getDialer()->getSocket()->peerAddress().toString().replace("::ffff:","")+ "开启监听\n");
#endif
    }

}

/*
*@说明：反转当前监听状态
*/
void CallingClientsUIItem::invertMonitor(){
    setMonitor(!this->monitor);
}

/*
*@说明：关闭该通话的监听功能
*/
void CallingClientsUIItem::closeMonitor(){
    this->getDialer()->setMonitor(false);
    this->getReceiver()->setMonitor(false);
    this->ui->button_Play->setStyleSheet("background-color: green;");
}

CallingClientsUIItem::~CallingClientsUIItem()
{
    qDebug()<<"本次通话时长："<<callingTime->toString();
    timerStop();
    disconnect(ui->button_Stop,&QPushButton::clicked,dialer,&ClientSocketItem::hangUPTheCall);
    disconnect(receiver,&ClientSocketItem::callingStatusChange,this,&CallingClientsUIItem::callingStatusChange);
    delete ui;

    delete timer;

    delete callingTime;
}
