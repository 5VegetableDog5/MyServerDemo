#include "clientsocketitem.h"

QByteArray* ClientSocketItem::dataFrameHeader = nullptr;
QByteArray* ClientSocketItem::ACKFrame = nullptr;
QByteArray* ClientSocketItem::callFrame = nullptr;
QByteArray* ClientSocketItem::hangupFrame = nullptr;
QByteArray* ClientSocketItem::callingBeginFrame = nullptr;

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

    //帧初始化（这些帧是用来回复给设备且固定的帧）
    if(!frameInited())
        initFrame();

    // 定时器初始化
    // 设置定时器的时间间隔为1秒
    timer->setInterval(1000);
    // 连接定时器的timeout()信号到自定义的槽函数
    connect(timer, &QTimer::timeout, this, &ClientSocketItem::timerTimeOut);

    //绑定槽函数
    connect(clientSocket,&QTcpSocket::readyRead,this,&ClientSocketItem::readTcpData);
    connect(clientSocket,&QTcpSocket::disconnected,this,&ClientSocketItem::handleCloseConnection);
    qDebug()<<"start new Thread!";
}

ClientSocketItem::ClientSocketItem(){

}


void ClientSocketItem::readTcpData(){
    static int errorNum = 0;

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

                if(!examineFrameHeader()){//检验帧头合法性

                    if(!adjustFrameHeader()){//如果调整失败
                        break;
                    }
                }

                //qDebug()<<"header"<<this->clientSocket->peerAddress() << " and Avaiablebytes = " <<clientSocket->bytesAvailable();
                //for (int i = 0; i < header.size(); i++) {
                //    qDebug().noquote() << QString::number(static_cast<quint8>(header.at(i)), 16).toUpper();
                //}

                //解析帧头
                if(header.at(0) == 0x00){//控制指令
                    //qDebug()<<"-------------------------------";
                    if(header.at(1) == 0x00){//1.注册指令
                        if(this->legality == false){
                            onLine();
                            qDebug()<<"online";
#if DEBUG
                            printfLog(clientSocket->peerAddress().toString().replace("::ffff:","")+" 上线 ");
#endif
                        }else{
                            /*if(!loginedFlag){
                                rebackACKFrame(1);
                            }
                            else{
                                cleanNeedlessZero();
                            }*/
                            rebackACKFrame(1);
                            //qDebug()<<"error 已经注册过了"<<this->clientSocket->peerAddress();
                        }
                    }else if(header.at(1) == 0x01){//2.拨号指令
                        if(this->status == AVAILABLE){
                            willreceive = IPDATA;
                            willReceiveLength = 15;
                            qDebug()<<getSocket()->peerAddress().toString().replace("::ffff:","")<<"拨号";
                        }else{
                            rebackACKFrame(1);
                            willreceive = IPDATA;
                            willReceiveLength = 15;
                            qDebug()<<getSocket()->peerAddress().toString().replace("::ffff:","")<<"请先结束当前通话！"<<this->clientSocket->peerAddress();
#if DEBUG
                            printfLog(getSocket()->peerAddress().toString().replace("::ffff:","")+"请先结束当前通话！"+this->clientSocket->peerAddress().toString());
#endif
                        }

                    }else if(static_cast<unsigned char>(header.at(1)) == static_cast<unsigned char>(0xCF)){//3.挂断指令
                        if(status == DIALSTATUS || status == ANSWERINGSTATUS){
                            static int i;

                            //要连续收到三次才挂断,此处读取后面两次是否未挂断指令
                            static QByteArray qba;
                            if(clientSocket->bytesAvailable() >= 2*2){
                                for(i = 0;i < 2;i++){
                                    qba = clientSocket->read(2);
                                    if(static_cast<unsigned char>(qba.at(0)) == static_cast<unsigned char>(0x00)
                                            &&static_cast<unsigned char>(qba.at(1)) == static_cast<unsigned char>(0xCF))
                                        continue;
                                    else break;
                                }
                                if(i == 2){
#if DEBUG
                                    printfLog(getSocket()->peerAddress().toString().replace("::ffff:","")+" 挂断与 " +
                                              targetClientItem->getSocket()->peerAddress().toString().replace("::ffff:","").replace("::ffff","")+"的通话");
#endif
                                    hangUPTheCall();
                                }
                            }else{
                                continue;
                            }


                            qDebug()<<getSocket()->peerAddress().toString().replace("::ffff:","")<<"hangup!";
                        }else{
#if DEBUG
                            printfLog(getSocket()->peerAddress().toString().replace("::ffff:","")+"hang up error! status error!");
#endif
                            qDebug()<<getSocket()->peerAddress().toString().replace("::ffff:","")<<"hang up error! status error!";
                        }

                    }else if(header.at(1) == 0x03){//4.下线指令
                        //offLine();
#if DEBUG
                            printfLog(getSocket()->peerAddress().toString().replace("::ffff:","")+"offLine");
#endif
                        qDebug()<<getSocket()->peerAddress().toString().replace("::ffff:","")<<"offLine";
                    }else if(header.at(1) == 0x11){//5.同意接听
                        if((this->status == DIALSTATUS ||this->status == ANSWERINGSTATUS)//确保是拨号状态
                                && AGREEANSWERING == 1){
                            qDebug()<<clientSocket->peerAddress().toString()<<"同意接听";
                            loginedFlag = true;//说明成功注册了
#if DEBUG
                            printfLog(clientSocket->peerAddress().toString()+"同意接听");
#endif
                            AGREEANSWERING = 2;
                            emit callingStatusChange(2);//通知UI同意接听
                            rebackACKFrame(3);
                            emit requestToSendFrame(3);
                            rebackACKFrame(3);
                            emit requestToSendFrame(3);
                            rebackACKFrame(3);
                            emit requestToSendFrame(3);
                        }else if(AGREEANSWERING == 2 && (this->status == DIALSTATUS ||this->status == ANSWERINGSTATUS)){
                            rebackACKFrame(3);
                            emit requestToSendFrame(3);
                        }

                    }
                    continue;
                }else if(header.at(0) == 0x01){
                    if(header.at(1) == 0x00){//音频数据帧
                        if(status == DIALSTATUS || status == ANSWERINGSTATUS){
                            willreceive = SOUNDDATA;
                            willReceiveLength = DATALENGTH;
                        }else{
                            qDebug()<<getSocket()->peerAddress().toString().replace("::ffff:","")<<"音频数据帧错误，当前还未进入通话状态!";
                        }

                    }else if(header.at(1) == 0x02){//IP数据帧
                        if(status == AVAILABLE){
                            willreceive = IPDATA;
                            willReceiveLength = 15;
                        }else{
#if DEBUG
                            printfLog(getSocket()->peerAddress().toString().replace("::ffff:","")+"IPDATA ERROR!!");
#endif
                            qDebug()<<getSocket()->peerAddress().toString().replace("::ffff:","")<<"IPDATA ERROR!!";
                        }

                    }else if(header.at(1) == 0x01){//测试用数据帧
                        //willreceive = 0x03;//暂时不用
                        //willReceiveLength = 16;
                    }
                }else{//error
                    errorNum++;
                    if(errorNum >=4){
                        //如果错误次数大于5次则跳一个比特
                        if(clientSocket->bytesAvailable() >= 1){
                            clientSocket->read(1);
                            qDebug()<<"error Num>5";
                        }
                        errorNum = 0;
                    }
                }
            }

            if(willreceive == IPDATA){//1.IP数据帧
                if(this->status == AVAILABLE && clientSocket->bytesAvailable() >= 15){
                    data = clientSocket->read(15);
                    if(this->legality){//确保已经注册且在线
                        strdata = new QString(data);
#if DEBUG
                        printfLog(this->clientSocket->peerAddress().toString()+" 对 "+(*strdata) + "进行拨号");
#endif
                        dial(removeLeadingZeros(*strdata));//尝试拨号
                    }

                }else{
                    clientSocket->read(15);
                    rebackACKFrame(1);
                    willreceive = HEADER;
                    willReceiveLength = 0;
                    continue;
                }
                willreceive = HEADER;
                willReceiveLength = 0;
                continue;
            }else if(willreceive == SOUNDDATA){//2.音频数据帧
                if(clientSocket->bytesAvailable() >=willReceiveLength){
                    //qDebug() <<clientSocket->peerAddress()<< "bytesAvaiable"<<this->clientSocket->bytesAvailable()<<"willreceive"<<willReceiveLength;
                    data = clientSocket->read(willReceiveLength);
                    if(targetClientItem){
                        //qDebug() << "音频数据";
                        if(status == DIALSTATUS || (status == ANSWERINGSTATUS && AGREEANSWERING == 2)){//确保是通话状态
                            requestSendDataFrameHeader();
                            emit requestToSend(data,data.length());
#if RECODE
                            //录音操作（这里录的是拨号方的声音）
                            if(this->status == DIALSTATUS && !sf_write(data)){
                                qDebug()<<"写入失败";
                            }
#endif
                        }
                    }else{
#if DEBUG
                            printfLog("program error!");
#endif
                        qDebug()<<"program error!";
                        qDebug()<<"the item "<<getSocket()->peerAddress().toString().replace("::ffff:","")<<getSocket()->peerPort()<<"is DIALSTATUS,but targetClientItem is NULL";
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
                            requestSendDataFrameHeader();
                            //targetClientItem->getSocket()->write(data,willReceiveLength);
                            emit requestToSend(data,data.length());
                            if(!sf_write(data)){
                                qDebug()<<"写入失败";
                            }
                        }
                    }else{
                        qDebug()<<"program error! <<";
                        qDebug()<<"the item "<<getSocket()->peerAddress().toString().replace("::ffff:","")<<getSocket()->peerPort()<<"is DIALSTATUS,but targetClientItem is NULL";
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

bool ClientSocketItem::adjustFrameHeader(){
    if(clientSocket){
        do{
            if(clientSocket->bytesAvailable() >= 1){
                /*qDebug()<<"---------------";
                for (int i = 0; i < header.size(); i++) {
                    qDebug().noquote() << QString::number(static_cast<quint8>(header.at(i)), 16).toUpper();
                }*/
                //调整帧头
                header.remove(0,1);//删除索引为0，长度为1的内容
                header.append(clientSocket->read(1));
            }else{
                //流中的比特数已经不足1 调整失败
                return false;
                break;
            }
        }while(!examineFrameHeader());
        return true;

    }else{
        return false;
    }

    return true;
}

bool ClientSocketItem::examineFrameHeader(){
    if(header.at(0) == 0x00)
    {
        switch(static_cast<unsigned char>(header.at(1))){
            case 0x00:return true;break;
            case 0x01:return true;break;
            case 0xCF:return true;break;
            case 0x03:return true;break;
            case 0x11:return true;break;
            default:return false;
        }
    }else if(header.at(0) == 0x01){
        switch(static_cast<unsigned char>(header.at(1))){
            case 0x00:return true;break;
            case 0x02:return true;break;
            case 0x01:return true;break;
            default:return false;
        }
    }
    return false;

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
        connect(this,&ClientSocketItem::requestToSendFrame,targetClientItem,&ClientSocketItem::rebackACKFrame);
        connect(targetClientItem,&ClientSocketItem::requestToSendFrame,this,&ClientSocketItem::rebackACKFrame);
        connect(this,&ClientSocketItem::requestToHangup,targetClientItem,&ClientSocketItem::hangUPed);
        connect(targetClientItem,&ClientSocketItem::requestToHangup,this,&ClientSocketItem::hangUPed);
        connect(this,&ClientSocketItem::requestToANSWER,targetClientItem,&ClientSocketItem::beginWaitANSER);

        //通知Server转发至UI
        emit call(this,targetClientItem);

        //返回应答信号
        rebackACKFrame(1);

        emit requestToANSWER();

        //告知接听设备
        if(callFrame)
            emit requestToSend(*callFrame,callFrame->count());

        loginedFlag = true;//说明成功注册了
#if RECODE
        beginRecording();//开始录音
#endif
        return true;

    }else{
        qDebug() << "target set error";
    }
    return false;
}

//定时任务触发函数
void ClientSocketItem::timerTimeOut(){
    if(AGREEANSWERING == 1){
        rebackACKFrame(2);
    }else{
        qDebug()<<"timer stop";
        timer->stop();
    }
}

/*
 *
 * 用于检验控制帧是否初始化完成
 * 返回值: 如果已经初始化成功则返回 true
 *        如果未 初 始化成功则返回 flase

*/
bool ClientSocketItem::frameInited(){
    if(dataFrameHeader == nullptr) return false;
    if(callFrame == nullptr) return false;
    if(hangupFrame == nullptr) return false;
    if(callingBeginFrame == nullptr) return false;
    return true;
}

void ClientSocketItem::initFrame(){
    dataFrameHeader = new QByteArray();
    callFrame = new QByteArray();
    hangupFrame = new QByteArray();
    callingBeginFrame = new QByteArray();
    if(ACKFrame == nullptr) ACKFrame = new QByteArray();

    //帧头
    dataFrameHeader->append(0x11);dataFrameHeader->append(0x11);
    callFrame->append(static_cast<int>(0x00));callFrame->append(static_cast<int>(0x01));
    hangupFrame->append(static_cast<int>(0x00));hangupFrame->append(static_cast<int>(0x10));

    //该帧用于通知拨号方已经拨通，可以开始通话了
    callingBeginFrame->append(static_cast<int>(0x00));callingBeginFrame->append(static_cast<int>(0x11));

    //普通应答信号
    ACKFrame->append(static_cast<int>(0x00));ACKFrame->append(static_cast<unsigned int>(0xac));

    //数据部分
    for(int i=0;i<DATALENGTH;i++){
        ACKFrame->append(static_cast<int>(0x00));
        hangupFrame->append(static_cast<int>(0x00));
        callFrame->append(static_cast<int>(0x00));
        callingBeginFrame->append(static_cast<int>(0x00));
    }

}

void ClientSocketItem::requestSendDataFrameHeader(){
    emit requestToSend(*dataFrameHeader,dataFrameHeader->count());
}

void ClientSocketItem::cleanNeedlessZero(){
    if(clientSocket){
        do{
            if(clientSocket->bytesAvailable() >= 1){
                /*qDebug()<<"---------------";
                for (int i = 0; i < header.size(); i++) {
                    qDebug().noquote() << QString::number(static_cast<quint8>(header.at(i)), 16).toUpper();
                }*/
                //调整帧头
                header.remove(0,1);//删除索引为0，长度为1的内容
                header.append(clientSocket->read(1));
            }else{
                //流中的比特数已经不足1 调整失败
                break;
            }
        }while(static_cast<unsigned char>(header.at(0)) == 0x00 && static_cast<unsigned char>(header.at(1)) == 0x00);

    }else{
    }

}

void ClientSocketItem::hangUPTheCall(){//主动挂电话

    AGREEANSWERING = 0;

    //返回应答信号
    //if(clientSocket->state()  != QAbstractSocket::UnconnectedState)
        rebackACKFrame(4);

    //告知对方设备
    emit requestToSend(*hangupFrame,hangupFrame->count());

    //通知Server 转发至UI
    emit hangUp(this);

    emit requestToHangup();
    setStatus(AVAILABLE);

    //断开信号函数和槽函数用于两个CLient之间的通讯
    disconnect(this,&ClientSocketItem::requestToSend,targetClientItem,&ClientSocketItem::sendData);
    disconnect(targetClientItem,&ClientSocketItem::requestToSend,this,&ClientSocketItem::sendData);
    disconnect(this,&ClientSocketItem::requestToHangup,targetClientItem,&ClientSocketItem::hangUPed);
    disconnect(targetClientItem,&ClientSocketItem::requestToHangup,this,&ClientSocketItem::hangUPed);
    disconnect(this,&ClientSocketItem::requestToSendFrame,targetClientItem,&ClientSocketItem::rebackACKFrame);
    disconnect(targetClientItem,&ClientSocketItem::requestToSendFrame,this,&ClientSocketItem::rebackACKFrame);
    disconnect(this,&ClientSocketItem::requestToANSWER,targetClientItem,&ClientSocketItem::beginWaitANSER);
    disconnect(targetClientItem,&ClientSocketItem::requestToANSWER,this,&ClientSocketItem::beginWaitANSER);

    setTatgetClientItem(nullptr);
    qDebug()<<"123";
}

void ClientSocketItem::hangUPed(){//被动挂电话
    AGREEANSWERING = 0;
    setStatus(AVAILABLE);
    setTatgetClientItem(nullptr);
}

void ClientSocketItem::beginWaitANSER(){
    if(AGREEANSWERING == 0){
        emit callingStatusChange(1);
        AGREEANSWERING = 1;//接听未应答状态
        // 启动定时器
        timer->start();
    }

};

void ClientSocketItem::onLine(){
    clientSocket->read(1);
    legality = true;
    rebackACKFrame(1);
    //检测是否有相同IP
    Server::searchSameIP(this);

    //将当前用户添加至 在线用户组
    Server::addOnlineClient(this);
    Server::showOnlineClients();
    emit onlineClientSingal(this->clientSocket->peerAddress().toString(),this->status);
}

void ClientSocketItem::offLine(){
    hangUPTheCall();
    if(this->status != AVAILABLE)
        emit hangUp(this);
    emit offLineSingal(clientSocket->peerAddress().toString());

    this->legality = false;

    Server::deleteOnlieClient(this);
}

bool ClientSocketItem::beginRecording(){
    //初始化录音文件
    fileInfo.channels = 2;             // 声道数
    fileInfo.samplerate = SAMPLERATE;       // 采样率
    fileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;  // WAV文件格式 16位色深


    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString timeStr = dateTime.toString("yyyy-MM-dd_hh-mm-ss");//格式化时间

    QString filePath;

    if(targetClientItem){
        filePath = "data/"+this->clientSocket->peerAddress().toString()+"_to_"+targetClientItem->getSocket()->peerAddress().toString().replace("::ffff:","")+" "+timeStr+".wav";
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
        if(!clientSocket->peerAddress().toString().isEmpty()){
            qDebug()<<clientSocket->peerAddress()<<clientSocket->peerPort()<<"close ...";
            //通知UI界面下线
            emit offLineSingal(clientSocket->peerAddress().toString());
            if(this->status != AVAILABLE)
                emit hangUp(this);
        }else{
            qDebug()<<"UI下线失败"<<clientSocket->peerAddress().toString();
        }


        if(this->status != AVAILABLE){
            hangUPTheCall();
        }

        if(clientSocket)
            clientSocket->close();


        //从在线设备中删除
        Server::deleteOnlieClient(this);
        Server::showOnlineClients();

        //释放内存
        if(clientSocket)
            clientSocket->deleteLater();

        //qDebug()<<"停止线程";
        clientThread->quit();
        //clientThread->wait(); // 等待线程执行完毕
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
    if(clientSocket){

        this->clientSocket->write(data,length);
    }
#if RECODE
    //录音操作（这里录的是接听方的声音）
    if(this->status == DIALSTATUS && !sf_write(data)){
        qDebug()<<"写入失败";
    }
#endif
}

/*
 * 函数功能：回复应答帧（给设备）
 *
 * 参数:
 * 可以是如下几种
 *      1 : 返回ACK应答信号
 *      2 : 返回提示对方来电信号
 *      3 : 返回提示对方接听信号
 *      4 : 返回提示对方挂机信号 00 10
 *
 *返回值:无
*/
void ClientSocketItem::rebackACKFrame(int num){
    switch(num){
        case 1:clientSocket->write(*ACKFrame,ACKFrame->count());break;
        case 2:clientSocket->write(*callFrame,callFrame->count());break;
        case 3:clientSocket->write(*callingBeginFrame,callingBeginFrame->count());break;
        case 4:clientSocket->write(*hangupFrame,hangupFrame->count());break;
    }
}


void ClientSocketItem::disconncetClient(){
    if(clientSocket){
        if(this->status != AVAILABLE){
            hangUPTheCall();
        }

        if(this->status != AVAILABLE)
            emit hangUp(this);

        //通知UI界面下线
        emit offLineSingal(clientSocket->peerAddress().toString());

        clientSocket->disconnectFromHost();
    }
}

QTcpSocket* ClientSocketItem::getSocket(){
    return this->clientSocket;
}
