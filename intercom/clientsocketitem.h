#ifndef CLIENTSOCKETITEM_H
#define CLIENTSOCKETITEM_H

#define AVAILABLE        0   //空闲状态
#define DIALSTATUS       1   //拨号状态
#define ANSWERINGSTATUS  2   //接听状态

#define HEADER 0//帧头
#define SOUNDDATA 1 //音频数据帧
#define IPDATA 2//IP数据帧
#define TESTDATA 3 //测试数据帧

#include <QTcpSocket>
#include <QThread>
#include <QEventLoop>
#include <QDateTime>
#include <QTimer>
#include <QFile>

#include "config.h"
#include "history.h"
#include "odbc.h"

#if RECODE
#include "recoder.h"
#endif
#if RECODE
#include "sndfile.h"
#endif

class ClientSocketItem : public QObject
{
    Q_OBJECT
public:

    //硬件控制帧
    //帧头:00 ac
    static QByteArray *ACKFrame;
    //帧头:11 11
    static QByteArray *dataFrameHeader;
    //帧头:00 01
    static QByteArray *callFrame;
    //帧头:00 10
    static QByteArray *hangupFrame;
    //帧头:00 11
    static QByteArray *callingBeginFrame;

    ClientSocketItem(QTcpSocket *clientSocket);
    ClientSocketItem();
    QTcpSocket* getSocket();
    short getStatus();
    void setStatus(short status);
    bool dial(const QString& targetIP);
    void disconncetClient();
    void hangUPTheCall();
    void hangUPed();
    void beginWaitANSER();


    //开始时间相关函数
    void setBeginTime(const QDateTime& time);
    QDateTime getBeginTime();

#if MONITOR
    void setMonitor(bool STATUS);
    bool getMonitor();
#endif

    QString removeLeadingZeros(const QString& ipAddress);

    QThread *clientThread;

    void distory();


public: signals:
    void requestToSend(const QByteArray data,int length);
    void requestToSendFrame(int num);
    void requestToANSWER();
    void onlineClientSingal(const QString ipAddr,const short status);
    void statusChanged(const QString ipAddr,const short newStatus);
    void offLineSingal(const QString ipAddr);
    void requestToHangup();
    void callingStatusChange(int status);

    void call(ClientSocketItem *dialer,ClientSocketItem *receiver);
    void hangUp(ClientSocketItem *dialer);

#if RECODE
        void requestSynchronousFilePtr(SNDFILE *file_dial,SNDFILE *file_answer);
#endif

#if MONITOR
    void requestPlayAudioDial(const QByteArray audioData);
    void requestPlayAudioAnswer(const QByteArray audioData);
    void testSingnal(const QByteArray audioData);
#endif

    //历史记录相关信号
    void requestSaveHistory(QString dialer,QString answer,int callDuration,QDateTime beginTime);
protected:

private:
    //void playAudio(const QByteArray& soundData);
    //合法性
    bool legality;

    //合法性验证次数
    short legalityCount;

    //通话状态
    short status;

    //登录状态
    bool loginedFlag = false;

    //开始通话时间
    QDateTime beginTime;


#if MONITOR

    //监听器 false：关闭； true：打开；
    bool monitor = false;
#endif

    //0:未启用 1:未应答 2:同意接听 3:拒绝接听
    short AGREEANSWERING = 0;

    short willreceive = 0;
    int willReceiveLength=0;
    QString *strdata;
    QByteArray data;//数据
    QByteArray header;//帧头

#if RECODE
    SF_INFO fileInfo;
    SNDFILE* file_dial;//录音文件(拨号方）
    SNDFILE* file_answer;//录音文件（接听方）
    QString filePath_dial;
    QString filePath_answer;
#endif

    QTcpSocket* clientSocket;
    ClientSocketItem* targetClientItem;

    //定时器
    QTimer *timer = new QTimer(this);



    bool adjustFrameHeader();
    bool examineFrameHeader();

#if RECODE
    //录音相关函数
    //删除文件夹
    bool deleteFile(const QString& filePath);
    void synchronousFilePtr(SNDFILE *file_dial,SNDFILE *file_answer);
#endif

    bool setTatgetClientItem(const QString& IPAddress_port);
    bool setTatgetClientItem(ClientSocketItem* targetItem);

    void timerTimeOut();

    void rebackACKFrame(int num);

    bool frameInited();
    void initFrame();
    void requestSendDataFrameHeader();
    void cleanNeedlessZero();
    void sendData(const QByteArray data,int length);

    void onLine();
    void offLine();
    bool beginRecording();

    //针对TCP流的处理函数
    bool handleIPDATA();



private slots:

    void readTcpData();
    void handleCloseConnection();
};

#endif // CLIENTSOCKETITEM_H
