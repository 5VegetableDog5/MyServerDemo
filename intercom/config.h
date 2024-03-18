#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QTextBrowser>
#include <QTextCursor>
#include <QMessageBox>
#include <QSqlQuery>

//时间类
#include <QDateTimeEdit>

//数据库操作头文件
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>

//外部库
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>


//1 开启debug , 0关闭debug
//开启则表示界面中会显示部分调试信息
#define DEBUG 1

//1 开启录音 ，  0关闭录音
#define RECODE 1

//监听功能 1：开启 0：关闭
#define MONITOR 1

const short LEGALITYMAXCOUNT = 5;   //最大合法性验证次数

const int DATALENGTH = 1024;//数据帧长度

//采样率
const static unsigned int SAMPLERATE = 6000;
// 声道数
const static unsigned short CHANNELS = 2;

#if DEBUG
    //全局方法，用于打印日志信息
    void printfLog(QString string);
#endif

#if MONITOR
    bool closeMONITOR();
#endif


#endif // CONFIG_H
