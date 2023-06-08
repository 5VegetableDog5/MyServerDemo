#ifndef ONLINECLIENTUIITEM_H
#define ONLINECLIENTUIITEM_H


#define AVAILABLE        0   //空闲状态
#define DIALSTATUS       1   //拨号状态
#define ANSWERINGSTATUS  2   //接听状态

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>


class OnlineClientUIItem : public QWidget
{
    Q_OBJECT
public:
    explicit OnlineClientUIItem(QWidget *parent,const QString IPAddr,const short status);
    void updataStatus(const short status);
    QString getIPAddr();
    short getStatus();


private:
    QHBoxLayout *qhBox;

    QLabel *ipLabel;
    QLabel *statusLabel;



signals:

};

#endif // ONLINECLIENTUIITEM_H
