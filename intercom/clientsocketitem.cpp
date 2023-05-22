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
        //将要收到的数据类型
        //0:帧头
        //1:音频数据帧
        //2:IP数据帧
//        static short willreceive = 0;

//        //如果当前要收到的数据帧的话，该值为将要收到的数据帧长度（字节）
//        static int willReceiveLength=0;

//        static QString *strdata;
//        static QByteArray data;//数据
//        static QByteArray header;//帧头
        while(clientSocket->bytesAvailable() >= 2){

            if(willreceive == HEADER){

                //读取帧头
                if(clientSocket->bytesAvailable() >= 2){
                    header = clientSocket->read(2);
                }else{
                    break;
                }
                qDebug()<<"header"<<this->clientSocket->peerAddress() << " and Avaiablebytes = " <<clientSocket->bytesAvailable();
                for (int i = 0; i < header.size(); ++i) {
                    qDebug().noquote() << QString::number(static_cast<quint8>(header.at(i)), 16).toUpper();
                }


                //解析帧头
                if(header.at(0) == 0x00){//控制指令
                    qDebug()<<"-------------------------------";
                    if(header.at(1) == 0x00){//1.注册指令
                        if(this->legality == false){
                            onLine();
                            qDebug()<<"online";
                        }else{
                            qDebug()<<"error 已经注册过了"<<this->clientSocket->peerAddress();
                        }
                    }else if(header.at(1) == 0x01){//2.拨号指令
                        if(this->status == AVAILABLE){
                            willreceive = IPDATA;
                            willReceiveLength = 15;
                            qDebug()<<"拨号";
                        }else{
                            qDebug()<<"请先结束当前通话！"<<this->clientSocket->peerAddress();
                        }

                    }else if(header.at(1) == 0x02){//3.挂断指令
                        hangUPTheCall();
                        qDebug()<<"hangup!";
                    }else if(header.at(1) == 0x03){//4.下线指令
                        offLine();
                        qDebug()<<"offLine";
                    }
                    return;
                }else if(header.at(0) == 0x01){
                    if(header.at(1) == 0x00){//音频数据帧
                        willreceive = SOUNDDATA;
                        willReceiveLength = 4096;
                    }else if(header.at(1) == 0x02){//IP数据帧
                        willreceive = IPDATA;
                        willReceiveLength = 15;
                    }else if(header.at(1) == 0x01){//测试用数据帧
                        willreceive = 0x03;//暂时不用
                        willReceiveLength = 16;
                    }
                }
            }

            if(willreceive == IPDATA){//1.IP数据帧
                if(clientSocket->bytesAvailable() >= 15){
                    data = clientSocket->read(15);
                    if(this->legality){//确保已经注册且在线
                        strdata = new QString(data);
                        dial(removeLeadingZeros(*strdata));//尝试拨号
                    }

                }else{
                    break;
                }
                willreceive = HEADER;
                willReceiveLength = 0;
                continue;
            }else if(willreceive == SOUNDDATA){//2.音频数据帧
                if(clientSocket->bytesAvailable() >=willReceiveLength){
                    qDebug() <<clientSocket->peerAddress()<< "bytesAvaiable"<<this->clientSocket->bytesAvailable()<<"willreceive"<<willReceiveLength;
                    data = clientSocket->read(willReceiveLength);
                    if(targetClientItem){
                        qDebug() << "音频数据" ;
                        if(status == DIALSTATUS || status == ANSWERINGSTATUS){//确保是通话状态
                            targetClientItem->getSocket()->write(header,2);
                            targetClientItem->getSocket()->write(data,willReceiveLength);
                        }
                    }else{
                        qDebug()<<"program error! <<";
                        qDebug()<<"the item "<<getSocket()->peerAddress().toString()<<getSocket()->peerPort()<<"is DIALSTATUS,but targetClientItem is NULL";
                    }
                    willreceive = HEADER;
                    willReceiveLength = 0;

                }else{
                    break;
                }
                continue;
            }else if(willreceive == TESTDATA){
                if(clientSocket->bytesAvailable() >= 16){
                    qDebug() <<clientSocket->peerAddress()<< "bytesAvaiable"<<this->clientSocket->bytesAvailable()<<"willreceive"<<willReceiveLength;
                    data = clientSocket->read(willReceiveLength);
                    if(targetClientItem){
                        qDebug() << "测试数据" ;
                        if(status == DIALSTATUS || status == ANSWERINGSTATUS){//确保是通话状态
                            targetClientItem->getSocket()->write(header,2);
                            targetClientItem->getSocket()->write(data,willReceiveLength);
                        }
                    }else{
                        qDebug()<<"program error! <<";
                        qDebug()<<"the item "<<getSocket()->peerAddress().toString()<<getSocket()->peerPort()<<"is DIALSTATUS,but targetClientItem is NULL";
                    }
                    willreceive = HEADER;
                    willReceiveLength = 0;
                }else{
                    break;
                }
                continue;
            }
        }



        //qDebug() <<clientSocket->peerAddress()<<clientSocket->peerPort()<< " receive Data:" << data;
        /*
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
                    qDebug() << clientSocket->peerAddress()<<clientSocket->peerPort()<<"已达到非法请求次数上限，已自动断开连接!!";
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
                //qDebug() << 1 ;
                targetClientItem->getSocket()->write(QByteArray::fromStdString(data.toStdString()));
            }else{
                qDebug()<<"program error! <<";
                qDebug()<<"the item "<<getSocket()->peerAddress().toString()<<getSocket()->peerPort()<<"is DIALSTATUS,but targetClientItem is NULL";
            }
            return;
        }

        //如果状态为未通话状态
        if(this->status == AVAILABLE){
            QString *strdata = new QString(data);
            QStringList stringlist = strdata->split(' ');
            //检验用户请求 首先判断是否有连续三个#号标识符
            if((data[0]!='#' || data[1]!='#' || data[2]!='#') || stringlist.size()!=2){
                qDebug() <<clientSocket->peerAddress()<< clientSocket->peerPort() << "无效请求1！" << status;

            }else{
                if(setTatgetClientItem(stringlist.at(1))){
                    qDebug() << "target set to" << targetClientItem->getSocket()->peerAddress() << targetClientItem->getSocket()->peerPort();

                    //如果成功找到对应IP则将本身状态转化为拨号状态 且要将对方设为接听状态
                    //拨号状态的 targetClientItem 为拨号的对象
                    //接听状态的 targetClientItem 为拨号的发起者
                    setStatus(DIALSTATUS);
                    targetClientItem->setStatus(ANSWERINGSTATUS);
                    targetClientItem->setTatgetClientItem(this);
                    targetClientItem->getSocket()->write("RX");

                }else{
                    qDebug() << "target set error";
                }
            }
        }

        //如果为接听状态
        if(status == ANSWERINGSTATUS){
            if(targetClientItem){
                //qDebug() << 1 ;
                targetClientItem->getSocket()->write(QByteArray::fromStdString(data.toStdString()));
            }else{
                qDebug()<<"program error! <<";
                qDebug()<<"the item "<<getSocket()->peerAddress().toString()<<getSocket()->peerPort()<<"is DIALSTATUS,but targetClientItem is NULL";
            }
            return;
        }*/



    }

}

bool ClientSocketItem::setTatgetClientItem(const QString& IPAddress_port){
    qDebug()<<"set target Address "<<IPAddress_port;
    targetClientItem = Server::getTargetClientFromOnline(IPAddress_port);
    if(targetClientItem){
        return true;
    }

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

bool ClientSocketItem::dial(const QString& targetIP){
    if(setTatgetClientItem(targetIP)){
        qDebug() << "target set to" << targetClientItem->getSocket()->peerAddress() << targetClientItem->getSocket()->peerPort();

        //如果成功找到对应IP则将本身状态转化为拨号状态 且要将对方设为接听状态
        //拨号状态的 targetClientItem 为拨号的对象
        //接听状态的 targetClientItem 为拨号的发起者
        setStatus(DIALSTATUS);
        targetClientItem->setStatus(ANSWERINGSTATUS);
        targetClientItem->setTatgetClientItem(this);
        targetClientItem->getSocket()->write("RX");
        return true;

    }else{
        qDebug() << "target set error";
    }
    return false;
}

void ClientSocketItem::hangUPTheCall(){//挂电话
    this->status = AVAILABLE;
    targetClientItem->setStatus(AVAILABLE);
    targetClientItem->setTatgetClientItem(NULL);
    setTatgetClientItem(NULL);
}

void ClientSocketItem::onLine(){
    legality = true;
    //将当前用户添加至 在线用户组
    Server::addOnlineClient(this);
    Server::showOnlineClients();
}

void ClientSocketItem::offLine(){
    this->legality = false;
    hangUPTheCall();
    Server::deleteOnlieClient(this);
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

QString ClientSocketItem::removeLeadingZeros(const QString& ipAddress) {
    QStringList ipParts = ipAddress.split('.'); // 将IP地址字符串拆分成每个字段
    for (int i = 0; i < ipParts.size(); i++) {
        ipParts[i] = QString::number(ipParts[i].toInt()); // 转换为整数并重新转换为字符串
    }

    return ipParts.join('.'); // 返回去除前导零后的修正后的IP地址字符串
}


void ClientSocketItem::disconncetClient(){
    if(clientSocket){
        clientSocket->disconnectFromHost();
    }
}

QTcpSocket* ClientSocketItem::getSocket(){
    return this->clientSocket;
}
