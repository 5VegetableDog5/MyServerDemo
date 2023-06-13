#include "clientsocketitem.h"

ClientSocketItem::ClientSocketItem(QTcpSocket *clientSocket):clientThread(new QThread)
{
    this->clientSocket = clientSocket;
    legality = false;
    legalityCount = 0;//合法性验证次数，如果大于5次则自动断开该连接
    status = AVAILABLE;//当前通话状态 0：未通话，1：拨号状态 2：接听状态
    targetClientItem = nullptr;

    moveToThread(clientThread);

    // 将 ClientSocketItem 对象移动到新线程中
    clientSocket->setParent(nullptr);
    clientSocket->moveToThread(clientThread);
    clientSocket->setParent(this);


    // 启动新线程
    clientThread->start();

    //绑定槽函数
    connect(clientSocket,&QTcpSocket::readyRead,this,&ClientSocketItem::readTcpData);
    connect(clientSocket,&QTcpSocket::disconnected,this,&ClientSocketItem::handleCloseConnection);
    qDebug()<<"start new Thread!";
}

ClientSocketItem::ClientSocketItem(){

}


void ClientSocketItem::readTcpData(){

    while(clientSocket->bytesAvailable() >= 2){
        if(clientSocket){

            /*if(this->status == DIALSTATUS || this->status == ANSWERINGSTATUS){
                if(targetClientItem){
                    data = clientSocket->read(clientSocket->bytesAvailable());
                    emit requestToSend(data,data.length());
                    if(this->status == DIALSTATUS)
                        qDebug()<<"Test1";
                    else
                        qDebug()<<"Test2";
                    continue;
                }

            }*/

            if(willreceive == HEADER){
                //读取帧头
                if(clientSocket->bytesAvailable() >= 2){
                    header = clientSocket->read(2);
                }else{
                    return;
                }
                qDebug()<<"header"<<this->clientSocket->peerAddress() << " and Avaiablebytes = " <<clientSocket->bytesAvailable();
                for (int i = 0; i < header.size(); i++) {
                    qDebug().noquote() << QString::number(static_cast<quint8>(header.at(i)), 16).toUpper();
                }
                QString s;


                //解析帧头
                if(header.at(0) == 0x00){//控制指令
                    //qDebug()<<"-------------------------------";
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
                            qDebug()<<getSocket()->peerAddress().toString()<<"拨号";
                        }else{
                            qDebug()<<getSocket()->peerAddress().toString()<<"请先结束当前通话！"<<this->clientSocket->peerAddress();
                        }

                    }else if(static_cast<unsigned char>(header.at(1)) == static_cast<unsigned char>(0xCF)){//3.挂断指令
                        if(status == DIALSTATUS || status == ANSWERINGSTATUS){
                            hangUPTheCall();
                            qDebug()<<getSocket()->peerAddress().toString()<<"hangup!";
                        }else{
                            qDebug()<<getSocket()->peerAddress().toString()<<"hang up error! status error!";
                        }

                    }else if(header.at(1) == 0x03){//4.下线指令
                        //offLine();
                        qDebug()<<getSocket()->peerAddress().toString()<<"offLine";
                    }
                    continue;
                }else if(header.at(0) == 0x01){
                    if(header.at(1) == 0x00){//音频数据帧
                        if(status == DIALSTATUS || status == ANSWERINGSTATUS){
                            willreceive = SOUNDDATA;
                            willReceiveLength = DATALENGTH;
                        }else{
                            qDebug()<<getSocket()->peerAddress().toString()<<"音频数据帧错误，当前还未进入通话状态!";
                        }

                    }else if(header.at(1) == 0x02){//IP数据帧
                        if(status == AVAILABLE){
                            willreceive = IPDATA;
                            willReceiveLength = 15;
                        }else{
                            qDebug()<<getSocket()->peerAddress().toString()<<"IPDATA ERROR!!";
                        }

                    }else if(header.at(1) == 0x01){//测试用数据帧
                        willreceive = 0x03;//暂时不用
                        willReceiveLength = 16;
                    }
                }
            }

            if(willreceive == IPDATA){//1.IP数据帧
                if(this->status == AVAILABLE && clientSocket->bytesAvailable() >= 15){
                    data = clientSocket->read(15);
                    if(this->legality){//确保已经注册且在线
                        strdata = new QString(data);
                        dial(removeLeadingZeros(*strdata));//尝试拨号
                    }

                }else{
                    return;
                }
                willreceive = HEADER;
                willReceiveLength = 0;
                continue;
            }else if(willreceive == SOUNDDATA){//2.音频数据帧
                //static short error1flag;
                if(clientSocket->bytesAvailable() >=willReceiveLength){
                    qDebug() <<clientSocket->peerAddress()<< "bytesAvaiable"<<this->clientSocket->bytesAvailable()<<"willreceive"<<willReceiveLength;
                    data = clientSocket->read(willReceiveLength);
                    if(targetClientItem){
                        qDebug() << "音频数据" ;
                        if(status == DIALSTATUS || status == ANSWERINGSTATUS){//确保是通话状态
                            //targetClientItem->getSocket()->write(header,2);
                            //emit requestToSend(header,header.length());
                            //targetClientItem->getSocket()->write(data,willReceiveLength);
                            emit requestToSend(data,data.length());
                            /*error1flag = targetClientItem->getSocket()->write(data,willReceiveLength);
                            if(error1flag==-1){
                                qDebug()<<"error1 occurd! clientsijetitem.cpp Line115";
                            }*/
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
                            //targetClientItem->getSocket()->write(header,2);
                            emit requestToSend(header,header.length());
                            //targetClientItem->getSocket()->write(data,willReceiveLength);
                            emit requestToSend(data,data.length());
                            if(!sf_write(data)){
                                qDebug()<<"写入失败";
                            }
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

        }else{
            break;
        }
    }

}

//将buffer内的数据写入到wav文件中
bool ClientSocketItem::sf_write(const QByteArray buffer){
    if(file){
        // 写入音频数据
        sf_count_t numFramesWritten = sf_write_raw(file, buffer.constData(), buffer.size());
        if (numFramesWritten < 0) {
            qDebug()<<"写入音频数据失败";
            sf_close(file);
            return false;
        }
    }else{
        return false;
    }

    return true;
}

bool ClientSocketItem::setTatgetClientItem(const QString& IPAddress_port){
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
    说明：改变该客户端的状态,如果由通话状态转入了空闲状态，则会停止当前录音；
    参数：short status:状态参数，可为以下三种
            AVAILABLE       空闲状态
            DIALSTATUS      拨号状态
            ANSWERINGSTATUS 接听状态
    返回值：无
*/
void ClientSocketItem::setStatus(short status){
    if(this->status == DIALSTATUS && status == AVAILABLE){
        // 关闭文件
        if(file){
            sf_close(file);
            file = nullptr;
        }

    }
    this->status = status;
    emit statusChanged(this->clientSocket->peerAddress().toString(),status);
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

        //连接信号函数和槽函数用于两个CLient之间的通讯
        connect(this,&ClientSocketItem::requestToSend,targetClientItem,&ClientSocketItem::sendData);
        connect(targetClientItem,&ClientSocketItem::requestToSend,this,&ClientSocketItem::sendData);
        connect(this,&ClientSocketItem::requestToHangup,targetClientItem,&ClientSocketItem::hangUPed);

        //emit requestToSend("RX",2);

        //通知Server转发至UI
        emit call(this,targetClientItem);

        beginRecording();//开始录音
        return true;

    }else{
        qDebug() << "target set error";
    }
    return false;
}

void ClientSocketItem::hangUPTheCall(){//主动挂电话



    //断开信号函数和槽函数用于两个CLient之间的通讯
    disconnect(this,&ClientSocketItem::requestToSend,targetClientItem,&ClientSocketItem::sendData);
    disconnect(targetClientItem,&ClientSocketItem::requestToSend,this,&ClientSocketItem::sendData);
    disconnect(this,&ClientSocketItem::requestToHangup,targetClientItem,&ClientSocketItem::hangUPed);

    //通知Server 转发至UI
    emit hangUp(this);

    emit requestToHangup();
    setStatus(AVAILABLE);

    setTatgetClientItem(NULL);
}

void ClientSocketItem::hangUPed(){//被动挂电话
    setStatus(AVAILABLE);
    setTatgetClientItem(NULL);
}

void ClientSocketItem::onLine(){
    legality = true;
    //将当前用户添加至 在线用户组
    Server::addOnlineClient(this);
    Server::showOnlineClients();
    emit onlineClientSingal(this->clientSocket->peerAddress().toString(),this->status);
}

void ClientSocketItem::offLine(){
    this->legality = false;
    hangUPTheCall();
    Server::deleteOnlieClient(this);
}

bool ClientSocketItem::beginRecording(){
    //初始化录音文件
    fileInfo.channels = 2;             // 声道数
    fileInfo.samplerate = 6000;       // 采样率
    fileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;  // WAV文件格式 16位色深


    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString timeStr = dateTime.toString("yyyy-MM-dd_hh-mm-ss");//格式化时间

    QString filePath;

    if(targetClientItem){
        filePath = "../data/"+this->clientSocket->peerAddress().toString()+"_to_"+targetClientItem->getSocket()->peerAddress().toString()+" "+timeStr+".wav";
        //filePath = timeStr+".wav";
    }else{
        return false;
    }
    filePath.replace("::ffff:", "");

    QByteArray filePathBytes = filePath.toUtf8();
    const char* filePathStr = filePathBytes.constData();

    this->file = sf_open(filePathStr, SFM_WRITE, &fileInfo);
    if (!file) {
        qDebug()<<"无法创建录音文件";
        return false;
    }

    return true;
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
        //通知UI界面下线
        emit offLineSingal(clientSocket->peerAddress().toString());

        //从在线设备中删除
        Server::deleteOnlieClient(this);
        Server::showOnlineClients();

        clientSocket->close();
        if(targetClientItem){
            targetClientItem->setStatus(AVAILABLE);
            targetClientItem->setTatgetClientItem(nullptr);
        }
        //释放内存
        clientSocket->deleteLater();
        clientThread->quit();
        clientThread->deleteLater();

        this->deleteLater();

    }
}

//消除IP地址前面多余的0
QString ClientSocketItem::removeLeadingZeros(const QString& ipAddress) {
    QStringList ipParts = ipAddress.split('.'); // 将IP地址字符串拆分成每个字段
    for (int i = 0; i < ipParts.size(); i++) {
        ipParts[i] = QString::number(ipParts[i].toInt()); // 转换为整数并重新转换为字符串
    }

    return ipParts.join('.'); // 返回去除前导零后的修正后的IP地址字符串
}

void ClientSocketItem::sendData(const QByteArray data,int length){
    this->clientSocket->write(data,length);
    if(!sf_write(data)){
        qDebug()<<"写入失败";
    }
}


void ClientSocketItem::disconncetClient(){
    if(clientSocket){
        clientSocket->disconnectFromHost();
    }
}

QTcpSocket* ClientSocketItem::getSocket(){
    return this->clientSocket;
}
