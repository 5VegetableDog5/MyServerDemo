#ifndef CALLINGCLIENTSUIITEM_H
#define CALLINGCLIENTSUIITEM_H

#include <QWidget>
#include<QTime>

#include "clientsocketitem.h"
#include "audioplayer.h"

namespace Ui {
class CallingClientsUIItem;
}

class CallingClientsUIItem : public QWidget
{
    Q_OBJECT

public:
    explicit CallingClientsUIItem(QWidget *parent = nullptr,ClientSocketItem *dialer=nullptr,ClientSocketItem *receiver=nullptr);
    ~CallingClientsUIItem();


    ClientSocketItem* getDialer();
    ClientSocketItem* getReceiver();

#if MONITOR
    void closeMonitor();
#endif

private:
    ClientSocketItem *dialer;
    ClientSocketItem *receiver;
    QTimer *timer;
    QTime *callingTime;

    Ui::CallingClientsUIItem *ui;

    void callingTimeInit();

    void timerInit();
    void timerStart();
    void timerStop();
    void timerTimeOut();

    void callingStatusChange(int status);

#if MONITOR
    //监听器 false：关闭； true：打开；
    bool monitor = false;

    void setMonitor(bool STATUS);
    void invertMonitor();
#endif

};

#endif // CALLINGCLIENTSUIITEM_H
