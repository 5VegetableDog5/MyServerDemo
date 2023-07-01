#ifndef CLIENTSOCKETITEM_H
#define CLIENTSOCKETITEM_H


#define LEGALITYMAXCOUNT 5   //最大合法性验证次数
#define AVAILABLE        0   //空闲状态
#define DIALSTATUS       1   //拨号状态
#define ANSWERINGSTATUS  2   //接听状态

#define HEADER 0//帧头
#define SOUNDDATA 1 //音频数据帧
#define IPDATA 2//IP数据帧
#define TESTDATA 3 //测试数据帧

#define DATALENGTH 256

#include <QTcpSocket>
#include <QThread>
#include <QEventLoop>
#include <QDateTime>
#include <QTimer>


#include <server.h>
#include "sndfile.h"


class Server;

class ClientSocketItem : public QObject
{
    Q_OBJECT
public:
    ClientSocketItem(QTcpSocket *clientSocket);
    ClientSocketItem();
    QTcpSocket* getSocket();
    short getStatus();
    void setStatus(short status);
    void disconncetClient();
    void hangUPTheCall();
    void hangUPed();
    void beginWaitANSER();

    QString removeLeadingZeros(const QString& ipAddress);

    QThread *clientThread;
    void sendData(const QByteArray data,int length);

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
protected:

private:
    bool legality;
    short legalityCount;
    short status;

    //0:未启用 1:未应答 2:同意接听 3:拒绝接听
    short AGREEANSWERING = 0;

    short willreceive = 0;
    int willReceiveLength=0;
    QString *strdata;
    QByteArray data;//数据
    QByteArray header;//帧头

    SF_INFO fileInfo;
    SNDFILE* file;//录音文件

    QTcpSocket* clientSocket;
    ClientSocketItem* targetClientItem;

    QTimer *timer = new QTimer(this);

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

    bool adjustFrameHeader();
    bool examineFrameHeader();


    bool sf_write(const QByteArray buffer);
    bool setTatgetClientItem(const QString& IPAddress_port);
    bool setTatgetClientItem(ClientSocketItem* targetItem);
    bool dial(const QString& targetIP);

    void timerTimeOut();

    void rebackACKFrame(int num);

    bool frameInited();
    void initFrame();
    void requestSendDataFrameHeader();


    void onLine();
    void offLine();
    bool beginRecording();



private slots:

    void readTcpData();
    void handleCloseConnection();
};

#endif // CLIENTSOCKETITEM_H
