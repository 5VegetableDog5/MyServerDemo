#include "server.h"

QList<ClientSocketItem*> Server::onlineClients;

Server::Server()
{

    server = new QTcpServer(this);

    //监听8888端口
    if (!server->listen(QHostAddress::Any, 8888)) {
        qDebug() << "Failed to start server \n";
    } else {
        qDebug() << "Server started on port" << server->serverPort() <<" " <<server->serverAddress() << "\n";
    }

    //绑定槽函数
    connect(server, &QTcpServer::newConnection, this, &Server::handleNewConnection);

}

void Server::addOnlineClient(ClientSocketItem* ClientSocketItem){

    onlineClients << ClientSocketItem;
}

void Server::deleteOnlieClient(ClientSocketItem* ClientSocketItem){
    onlineClients.removeOne(ClientSocketItem);

}

/*
    说明：根据IP地址从在线客户端中获取其Item
    参数：QString s,例：192.168.1.1:8080
    返回值：如果找到了返回对应的Item,否则返回NULL

    2023.2.13修改说明：取消对端口号的限制，即ip正确即可
*/
ClientSocketItem* Server::getTargetClientFromOnline(QString targetIP){

    QHostAddress *targetIPAddress = new QHostAddress(targetIP);
    qDebug() << targetIPAddress->toIPv4Address() <<" "<<" Server!";

    for(int i = 0;i<onlineClients.size();i++){
        //qDebug()<<onlineClients[i]->getSocket()->peerAddress() <<" "<<onlineClients[i]->getSocket()->peerPort();
        if(onlineClients[i]->getSocket()->peerAddress().toIPv4Address() == targetIPAddress->toIPv4Address()
                && onlineClients[i]->getStatus() == AVAILABLE){
            return onlineClients[i];
        }
    }

    return NULL;
}

void Server::showOnlineClients(){
    qDebug() <<"当前在线客户端：" << onlineClients.size();
    for(int i = 0;i<onlineClients.size();i++){
        qDebug() << '['<<i<<"] "<< onlineClients[i]->getSocket()->peerAddress() << onlineClients[i]->getSocket()->peerPort();
    }
}

void Server::handleNewConnection()
{
    //处理新连接
    clientSocket = server->nextPendingConnection();
    qDebug() << "new client connected:" << clientSocket->peerAddress().toString()<<clientSocket->peerPort();

    ClientSocketItem *cst = new ClientSocketItem(clientSocket);
    for(int i = 0;i<onlineClients.size();i++){
        if(onlineClients[i]->getSocket()->peerAddress().toIPv4Address() == clientSocket->peerAddress().toIPv4Address()){
            qDebug()<<"删除相同的IP";
            onlineClients[i]->disconncetClient();
        }
    }
}

void Server::readTcpData(){
    if(clientSocket){
        QByteArray data = clientSocket->readLine();
        qDebug() << "Server receive Data:" << data;

        //检验用户请求
        if(data[0]!='#' || data[1]!='#' || data[2]!='#'){
            qDebug() << clientSocket->peerPort() << "请求不合法！\r\n";

        }else{

        }
        clientSocket->write("Hello client\r\n");
    }

}

void Server::handleCloseConnection(){
    if(clientSocket){
        qDebug()<<clientSocket->peerAddress()<<clientSocket->peerPort()<<"close ...";

        clientSocket->close();
        qDebug() << "OK\r\n";
        //释放内存
        clientSocket->deleteLater();
    }
}


