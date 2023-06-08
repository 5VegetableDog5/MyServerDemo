#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDebug>
#include <QFrame>

#include "onlineclientuiitem.h"
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void test();

    void addNewOnlineClient(const QString ipAddr,const short status);
    void upgradeOnlineClient(const QString ipAddr,const short newStatus);
    void offLineClient(const QString ipAddr);

private:
    Ui::MainWindow *ui;
    Server *server;

    QScrollArea *onlineScrollArea;
    QVBoxLayout *onlineVBoxLayout;

    QList<OnlineClientUIItem*> uiOnlineClient;

};
#endif // MAINWINDOW_H
