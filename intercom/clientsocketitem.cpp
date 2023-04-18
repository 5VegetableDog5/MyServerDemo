#include "clientsocketitem.h"

ClientSocketItem::ClientSocketItem(QTcpSocket *clientSocket)
{
    this->clientSocket = clientSocket;
    legality = false;
    legalityCount = 0;//合法性验证次数，如果大于5次则自动断开该连接
    status = AVAILABLE;//当前通话状态 0：未通话，1：拨号状态 2：接听状态
    targetClientItem = NULL;

    //绑定槽函数
    connect(clientSocket,&QTcpSocket::readyRead,this,&ClientSocketItem::readTcpData);
    connect(clientSocket,&QTcpSocket::disconnected,this,&ClientSocketItem::handleCloseConnection);
}



void ClientSocketItem::readTcpData(){

    if(clientSocket){
        QString data = clientSocket->readLine();
        qDebug() <<clientSocket->peerAddress()<<clientSocket->peerPort()<< " receive Data:" << data;

        //如果还没有通过合法性验证
        if(!legality){
            //请求格式:### (192.168.x.x:xxxx)
            //如果有IP地址，则尝试连接目标IP,否则只作为在线用户等待被连接
            //qDebug() << "legalityCount " <<legalityCount;

            //检验用户请求 首先判断是否有连续三个#号标识符
            if(data[0]!='#' || data[1]!='#' || data[2]!='#'){
                qDebug() <<clientSocket->peerAddress()<< clientSocket->peerPort() << "请求不合法！";
                //如果超过合法性验证次数
                if(legalityCount+1 >= LEGALITYMAXCOUNT){
                    disconncetClient();
                    return;
                }
                legalityCount++;
                return;
            }else{
                legality = true;
                //将当前用户添加至 在线用户组
                Server::addOnlineClient(this);
                Server::showOnlineClients();

                //targetClientSocket = Server::getTargetClientFromOnline()
            }


        }
        //如果为拨号状态
        if(status == DIALSTATUS){
            if(targetClientItem){
                targetClientItem->getSocket()->write(data.toUtf8());
            }else{
                qDebug()<<"program error! <<";
                qDebug()<<"the item "<<getSocket()->peerAddress().toString()<<getSocket()->peerPort()<<"is DIALSTATUS,but targetClientItem is NULL";
            }
            return;
        }

        //如果状态为未通话状态
        if(status == AVAILABLE){
            QStringList stringlist = data.split(' ');
            //检验用户请求 首先判断是否有连续三个#号标识符
            if((data[0]!='#' || data[1]!='#' || data[2]!='#') || stringlist.size()!=2){
                qDebug() <<clientSocket->peerAddress()<< clientSocket->peerPort() << "无效请求！";

            }else{
                if(setTatgetClientItem(stringlist.at(1))){
                    qDebug() << "target set to" << targetClientItem->getSocket()->peerAddress() << targetClientItem->getSocket()->peerPort();

                    //如果成功找到对应IP则将本身状态转化为拨号状态 且要将对方设为接听状态
                    //拨号状态的 targetClientItem 为拨号的对象
                    //接听状态的 targetClientItem 为拨号的发起者
                    setStatus(DIALSTATUS);
                    targetClientItem->setStatus(ANSWERINGSTATUS);
                    targetClientItem->setTatgetClientItem(this);

                }else{
                    qDebug() << "target set error";
                }
            }
        }

        //如果为接听状态
        if(status == ANSWERINGSTATUS){

            QStringList stringlist = data.split(' ');
            qDebug()<<"----------------------------------" << stringlist.at(1);
            //检验用户请求 首先判断是否有连续三个#号标识符
            if((data[0]!='#' || data[1]!='#' || data[2]!='#') || stringlist.size()!=2){
                qDebug() <<clientSocket->peerAddress()<< clientSocket->peerPort() << "无效请求！";
                return;
            }

            //如果是结束指令，结束指令只能由接听状态发起
            if(getStatus()==ANSWERINGSTATUS && (stringlist.at(1) == "OVERCALL")){
                //中断这两个客户端的连接
                qDebug() << getSocket()->peerAddress().toString() << getSocket()->peerPort()
                         << " OVERCALL " << targetClientItem->getSocket()->peerAddress().toString() << targetClientItem->getSocket()->peerPort();
                targetClientItem->setStatus(AVAILABLE);
                targetClientItem->setTatgetClientItem(NULL);
                this->setStatus(AVAILABLE);
                this->setTatgetClientItem(NULL);
            }
        }



    }

}

bool ClientSocketItem::setTatgetClientItem(QString IPAddress_port){
    qDebug()<<"set target Address "<<IPAddress_port;
    targetClientItem = Server::getTargetClientFromOnline(IPAddress_port);
    if(targetClientItem) return true;

    return false;
}

bool ClientSocketItem::setTatgetClientItem(ClientSocketItem* targetItem){
    this->targetClientItem = targetItem;
    return true;
}

/*
    说明：改变该客户端的状态；
    参数：short status:状态参数，可为以下三种
         AVAILABLE       空闲状态
         DIALSTATUS      拨号状态
         ANSWERINGSTATUS 接听状态
    返回值：无
*/
void ClientSocketItem::setStatus(short status){
    this->status = status;
}

short ClientSocketItem::getStatus(){
    return status;
}

/*
    说明：处理关闭客户端的操作
            1.从在线设备中删除
            2.如果有tagetClientItem则需要释放掉
            3.释放内存
*/
void ClientSocketItem::handleCloseConnection(){
    if(clientSocket){
        qDebug()<<clientSocket->peerAddress()<<clientSocket->peerPort()<<"close ...";

        //从在线设备中删除
        Server::deleteOnlieClient(this);
        Server::showOnlineClients();

        clientSocket->close();
        if(targetClientItem){
            targetClientItem->setStatus(AVAILABLE);
            targetClientItem->setTatgetClientItem(NULL);
        }

        //释放内存
        clientSocket->deleteLater();
        this->deleteLater();

    }
}

void ClientSocketItem::disconncetClient(){
    if(clientSocket){
        qDebug() << clientSocket->peerAddress()<<clientSocket->peerPort()<<"已达到非法请求次数上限，已自动断开连接!!";
        clientSocket->disconnectFromHost();
    }
}

QTcpSocket* ClientSocketItem::getSocket(){
    return this->clientSocket;
}
