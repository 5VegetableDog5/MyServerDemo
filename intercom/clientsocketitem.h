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

#include <QTcpSocket>
#include <QThread>

#include <server.h>

class Server;

class ClientSocketItem : public QObject
{
    Q_OBJECT
public:
    ClientSocketItem(QTcpSocket *clientSocket);
    QTcpSocket* getSocket();
    short getStatus();
    void setStatus(short status);
    void disconncetClient();
    QString removeLeadingZeros(const QString& ipAddress);

protected:

private:
    bool legality;
    short legalityCount;
    short status;

    short willreceive = 0;
    int willReceiveLength=0;
    QString *strdata;
    QByteArray data;//数据
    QByteArray header;//帧头

    QTcpSocket* clientSocket;
    ClientSocketItem* targetClientItem;


    bool setTatgetClientItem(const QString& IPAddress_port);
    bool setTatgetClientItem(ClientSocketItem* targetItem);
    bool dial(const QString& targetIP);
    void hangUPTheCall();
    void onLine();
    void offLine();

private slots:
    void readTcpData();
    void handleCloseConnection();
};

#endif // CLIENTSOCKETITEM_H
