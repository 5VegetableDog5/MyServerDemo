#ifndef CLIENTSOCKETITEM_H
#define CLIENTSOCKETITEM_H


#define LEGALITYMAXCOUNT 5   //最大合法性验证次数
#define AVAILABLE        0   //空闲状态
#define DIALSTATUS       1   //拨号状态
#define ANSWERINGSTATUS  2   //接听状态

#define HEADER 0//帧头
#define SOUNDDATA 1 //音频数据帧
#define IPDATA 2//IP数据帧

#include <QTcpSocket>

#include <server.h>

class Server;

class ClientSocketItem : public QObject
{
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
