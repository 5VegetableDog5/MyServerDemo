#ifndef ODBC_H
#define ODBC_H

#include <QString>
#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>
#include <QObject>

#include "config.h"

class ODBC :public QObject
{
    Q_OBJECT
public:
    ODBC();

public slots:
    //保存历史记录相关函数
    void saveNewHistory(QString dialer,QString answer,int callDuration,QDateTime beginTime);
};

#endif // ODBC_H
