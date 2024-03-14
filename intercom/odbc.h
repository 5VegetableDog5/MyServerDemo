#ifndef ODBC_H
#define ODBC_H

#include <QString>
#include <QSqlDatabase>
#include <QDebug>

#include "config.h"

class ODBC
{
public:
    ODBC(QString hostName,short PORT,QString databaseName,QString username,QString password);
};

#endif // ODBC_H
