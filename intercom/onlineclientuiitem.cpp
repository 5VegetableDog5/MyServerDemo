#include "onlineclientuiitem.h"

OnlineClientUIItem::OnlineClientUIItem(QWidget *parent,const QString IPAddr,const short status)
    : QWidget{parent}
{
    this->qhBox = new QHBoxLayout();

    this->ipLabel = new QLabel(IPAddr);

    switch(status){
        case AVAILABLE: this->statusLabel = new QLabel("AVAILABLE");break;
        case DIALSTATUS:this->statusLabel = new QLabel("DIALSTATUS");break;
        case ANSWERINGSTATUS:this->statusLabel = new QLabel("ANSWERINGSTATUS");break;
        default:this->statusLabel = new QLabel("error");
    }


    qhBox->addWidget(ipLabel);
    qhBox->addWidget(statusLabel);


    setLayout(qhBox);
}

void OnlineClientUIItem::updataStatus(const short status){
    switch(status){
        case AVAILABLE: this->statusLabel->setText("AVAILABLE");break;
        case DIALSTATUS:this->statusLabel->setText("DIALSTATUS");;break;
        case ANSWERINGSTATUS:this->statusLabel->setText("ANSWERINGSTATUS");break;
        default:this->statusLabel = new QLabel("error");
    }
    this->update();
}

QString OnlineClientUIItem::getIPAddr(){
    return this->ipLabel->text();
}

short OnlineClientUIItem::getStatus(){
    return this->statusLabel->text().toShort();
}

