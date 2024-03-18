#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDebug>
#include <QFrame>
#include <QTextBrowser>

#include "onlineclientuiitem.h"
#include "callingclientsuiitem.h"
#include "clientsocketitem.h"
#include "server.h"
#include "config.h"
#include "history.h"
#include "odbc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#if DEBUG
void printfLog(QString string);
#endif


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void test();

    void labelInit();

    //在线用户操作
    void addNewOnlineClient(const QString ipAddr,const short status);
    void upgradeOnlineClient(const QString ipAddr,const short newStatus);
    void offLineClient(const QString ipAddr);

    //当前通话操作
    void addNewCalling(ClientSocketItem *dialer,ClientSocketItem *receiver);
    void deleteCalling(ClientSocketItem *dialer);

    //按钮槽事件
    void pageChanged(int page);
    void showOnlineClients();
    void showCallingClients();


private:
    Ui::MainWindow *ui;
    Server *server;

    QVBoxLayout *onlineVBoxLayout;
    QVBoxLayout *callingVBoxLayout;

    QList<OnlineClientUIItem*> uiOnlineClient;

    void InitConnect();

    //历史记录窗口相关
    bool historyShow = false;

    void showHistoryUI();
    void setHistoryFlagClose();

};
#endif // MAINWINDOW_H
