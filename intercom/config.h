#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QTextBrowser>
#include <QTextCursor>



//1 开启debug , 0关闭debug
#define DEBUG 1

//q 开启录音 ，  0关闭录音
#define RECODE 0

const short LEGALITYMAXCOUNT = 5;   //最大合法性验证次数

const int DATALENGTH = 1024;//数据帧长度

//采样率
const static unsigned int SAMPLERATE = 6000;


#if DEBUG
    //全局方法，用于打印日志信息
    void printfLog(QString string);
#endif

#endif // CONFIG_H
