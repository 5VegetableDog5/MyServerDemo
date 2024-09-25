#ifndef NEWCONNCETIONUI_H
#define NEWCONNCETIONUI_H

#include <QWidget>
#include <QMessageBox>

#include "clientsocketitem.h"
#include "server.h"


namespace Ui {
class NewConncetionUI;
}

class NewConncetionUI : public QWidget
{
    Q_OBJECT

public:
    explicit NewConncetionUI(QWidget *parent = nullptr,Server* uiOnlineClient=nullptr);
    ~NewConncetionUI();

signals:
    void aboutToClose();

    void DialSignal(QString IPaddr);

protected:
    //重写closeEvent方法
    void closeEvent(QCloseEvent *event) override {
        emit aboutToClose();
        QWidget::closeEvent(event);
    }

private:
    Server* server;
    Ui::NewConncetionUI *ui;

    bool connect();

    void closeByButton();
};

#endif // NEWCONNCETIONUI_H
