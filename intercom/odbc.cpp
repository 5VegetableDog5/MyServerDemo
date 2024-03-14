#include "odbc.h"

//全局变量（数据库）
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

ODBC::ODBC(QString hostName,short port,QString databaseName,QString username,QString password){

    //配置数据库信息
    db.setHostName(hostName);
    db.setPort(port);
    db.setDatabaseName(databaseName);
    db.setUserName(username);
    db.setPassword(password);

#if DEBUG
    printfLog("连接数据库中...");
#endif

    // 打开数据库连接
    if (!db.open()) {
        qDebug() << "Failed to connect to database!";
#if DEBUG
        printfLog("失败！");
#endif
        return;
    }else{
#if DEBUG
        printfLog("成功！");
#endif
    }

}
