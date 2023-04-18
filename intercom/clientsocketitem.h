#ifndef CLIENTSOCKETITEM_H
#define CLIENTSOCKETITEM_H


#define LEGALITYMAXCOUNT 5   //最大合法性验证次数
#define AVAILABLE        0   //空闲状态
#define DIALSTATUS       1   //拨号状态
#define ANSWERINGSTATUS  2   //接听状态

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

protected:

private:
    bool legality;
    short legalityCount;
    short status;
    QTcpSocket* clientSocket;
    ClientSocketItem* targetClientItem;

    void disconncetClient();
    bool setTatgetClientItem(QString IPAddress_port);
    bool setTatgetClientItem(ClientSocketItem* targetItem);

private slots:
    void readTcpData();
    void handleCloseConnection();
};

#endif // CLIENTSOCKETITEM_H
