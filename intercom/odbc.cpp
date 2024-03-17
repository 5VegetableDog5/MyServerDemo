#include "odbc.h"

//全局变量（数据库）
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

ODBC::ODBC(){

    // 创建 QSettings 对象，指定INI文件路径
    QSettings settings(":/database/config.ini", QSettings::IniFormat);

    //配置数据库信息
    db.setHostName(settings.value("Database/host").toString());
    db.setPort(settings.value("Database/port").toInt());
    db.setDatabaseName(settings.value("Database/database").toString());
    db.setUserName(settings.value("Database/username").toString());
    db.setPassword(settings.value("Database/password").toString());

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

void ODBC::saveNewHistory(QString dialer,QString answer,int callDuration,QDateTime beginTime){
    QString insertQuery = "INSERT INTO history (dialer, answer, callDuration,beginTime) VALUES (:dialer, :answer, :callDuration,:beginTime)";

    QSqlQuery query;
    query.prepare(insertQuery);
    query.bindValue(":dialer", dialer);
    query.bindValue(":answer", answer);
    query.bindValue(":callDuration", (int)callDuration);
    query.bindValue(":beginTime", beginTime);
    qDebug()<<"通话时长："<< callDuration;

    if (query.exec()) {
        // 插入成功
    } else {
        // 插入失败，显示错误信息
        qDebug() << "插入数据失败：" << query.lastError().text();
#if DEBUG
        printfLog("历史记录添加失败！");
#endif
    }


}
