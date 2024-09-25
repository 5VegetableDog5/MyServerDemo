#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDir>


#include "clientsocketitem.h"
#include "config.h"
#include "odbc.h"
#include "audioplayer.h"

class ClientSocketItem;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    static void addOnlineClient(ClientSocketItem* ClientSocketItem);
    static void deleteOnlieClient(ClientSocketItem* ClientSocketItem);
    static ClientSocketItem* getTargetClientFromOnline(QString s);
    static void showOnlineClients();
    static void searchSameIP(ClientSocketItem *client);

    void emitNewClientSingals(const QString ipAddr,const short status);
    void emitUpgradeClientStatus(const QString ipAddr,const short newStatus);
    void emitOffLineSingal(const QString ipAddr);

    void emitNewCalling(ClientSocketItem *dialer,ClientSocketItem *receiver);
    void emitDeleteCalling(ClientSocketItem *dialer);



public: signals:
    void newOnlineClient(const QString ipAddr,const short status);
    void upgradeClientStatus(const QString ipAddr,const short newStatus);
    void offLineSingal(const QString ipAddr);

    void newCalling(ClientSocketItem *dialer,ClientSocketItem *receiver);
    void deleteCalling(ClientSocketItem *dialer);
    void distoryIt();
protected:


private:
    QTcpServer *server = NULL;
    QTcpSocket *clientSocket = NULL;
    static QList<ClientSocketItem*> onlineClients;

    ODBC *odbc;

private slots:
    void readTcpData();
    void handleNewConnection();
    void handleCloseConnection();
    bool creatDir(QString path);
};

#endif // SERVER_H
