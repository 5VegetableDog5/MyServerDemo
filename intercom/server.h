#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

#include <clientsocketitem.h>

class ClientSocketItem;

class Server : public QTcpServer
{
public:
    Server();
    static void addOnlineClient(ClientSocketItem* ClientSocketItem);
    static void deleteOnlieClient(ClientSocketItem* ClientSocketItem);
    static ClientSocketItem* getTargetClientFromOnline(QString s);
    static void showOnlineClients();


protected:


private:
    QTcpServer *server = NULL;
    QTcpSocket *clientSocket = NULL;
    static QList<ClientSocketItem*> onlineClients;

private slots:
    void readTcpData();
    void handleNewConnection();
    void handleCloseConnection();
};

#endif // SERVER_H
