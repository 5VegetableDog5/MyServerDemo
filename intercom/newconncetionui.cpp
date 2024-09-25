#include "newconncetionui.h"
#include "ui_newconncetionui.h"


NewConncetionUI::NewConncetionUI(QWidget *parent,Server* server)
    : QWidget(parent)
    , ui(new Ui::NewConncetionUI)
{
    ui->setupUi(this);

    this->server = server;

    QObject::connect(ui->pushButton_connect,&QPushButton::clicked,this,&NewConncetionUI::connect);

    QObject::connect(ui->pushButton_close, &QPushButton::clicked, this, &NewConncetionUI::closeByButton);


}

NewConncetionUI::~NewConncetionUI()
{
    delete ui;
}



bool NewConncetionUI::connect(){

    //QString str1 = ui->lineEdit_1->text();
    //QString str2 = ui->lineEdit_2->text();
    QString str1 = "192.168.2.52";
    QString str2 = "192.168.2.83";

    if(str1.isEmpty() || str2.isEmpty()) {
        QMessageBox::critical(nullptr, "错误", "输入为空!");
        return false;
    }

    ClientSocketItem *client1=nullptr ,*client2=nullptr;

    client1 = server->getTargetClientFromOnline(str1);
    client2 = server->getTargetClientFromOnline(str2);

    QObject::connect(this,&NewConncetionUI::DialSignal,client1,&ClientSocketItem::dial);


    if(client1!=nullptr && client2!=nullptr){
        if(client1->getStatus()== AVAILABLE && client2->getStatus() == AVAILABLE ){
            if(1){
                emit DialSignal(str2);
            }else{
                QMessageBox::critical(nullptr, "错误", "未知错误!");
            }
        }else{
            QMessageBox::critical(nullptr, "错误", "用户繁忙!");
        }
    }else{
        QMessageBox::critical(nullptr, "错误", "用户当前不在线!");
    }




    return false;
}

void NewConncetionUI::closeByButton(){
    this->close();
}
