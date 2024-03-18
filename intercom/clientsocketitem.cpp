#include "clientsocketitem.h"
#include "server.h"

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
                        }else{//挂断错误
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

                            //记录开始时间
                            beginTime = QDateTime::currentDateTime();
                            targetClientItem->setBeginTime(beginTime);//同步开始通话时间
#if RECODE
                            beginRecording();//开始录音
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
                    if(errorNum >=LEGALITYMAXCOUNT){
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
                handleIPDATA();
                continue;
            }else if(willreceive == SOUNDDATA){//2.音频数据帧
                if(clientSocket->bytesAvailable() >=willReceiveLength){
                    //qDebug() <<clientSocket->peerAddress()<< "bytesAvaiable"<<this->clientSocket->bytesAvailable()<<"willreceive"<<willReceiveLength;
                    data = clientSocket->read(willReceiveLength);
                    if(targetClientItem){
                        //qDebug() << "音频数据";
                        if(status == DIALSTATUS || (status == ANSWERINGSTATUS && AGREEANSWERING == 2)){//确保是通话状态
                            //发送音频数据至对方
                            requestSendDataFrameHeader();
                            emit requestToSend(data,data.length());
#if MONITOR
                            //如果该通话被监听
                            if(monitor){
                                playSound(data,CHANNELS,SAMPLERATE);
                            }
#endif

#if RECODE
                            //录音操作（这里录的是拨号方的声音）
                            if(this->status == DIALSTATUS && !sf_write(data,file_dial)){
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
/*#if RECODE
                            if(!sf_write(data)){
                                qDebug()<<"写入失败";
                            }
#endif*/
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

/*
 * @playSound说明：播放soundDate二进制音频数据
 * 参数：@soundData：二进制音频数据
 *      @channels：声道数
 *      @samlerate：波特率
 * 返回值：无
 *
*/
void ClientSocketItem::playSound(QByteArray soundData,unsigned int const channels,unsigned int const samplerate){

    if(soundData.size() == 0){
        qDebug()<<"音频播放失败！soundData 为 0 ！";
        return;
    }
    // 从QByteArray中获取音频数据
    const qint16* data = reinterpret_cast<const qint16*>(soundData.constData());
    std::size_t dataSize = soundData.size() / sizeof(qint16);


    // 从文件中加载音频数据
    if (!soundBuffer.loadFromSamples(data, dataSize ,channels,samplerate)) {
        // 将音频数据设置到音频对象中
        sound.setBuffer(soundBuffer);

        // 播放音频
        sound.play();

        // 等待音频播放完毕
        //while (sound.getStatus() == sf::Sound::Playing) {}
    }else{
        qDebug()<<"ClientSocketItem::playSound :加载音频数据失败！";
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

#if RECODE
//将buffer内的数据写入到wav文件中
bool ClientSocketItem::sf_write(const QByteArray buffer, SNDFILE* file){
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

/*
 * @mergeAudio
 * 说明:合并两段wav音频，需要保证两段音频的格式是相同的如波特率等
 * 参数：@input1:第一个wav文件的文件路径
 *      @input2:第二个wav文件的文件路径
 *      @output:输出文件的文件路径
 * 返回值:无
*/
void ClientSocketItem::mergeAudio(const QString& input1, const QString& input2, const QString& output){
    SNDFILE* infile1 = nullptr;
    SNDFILE* infile2 = nullptr;
    SF_INFO info1;
    SF_INFO info2;

    // 打开第一段音频文件
    infile1 = sf_open(input1.toStdString().c_str(), SFM_READ, &info1);
    if (!infile1) {
        qDebug() << "Error opening file" << input1;
        return;
    }

    // 打开第二段音频文件
    infile2 = sf_open(input2.toStdString().c_str(), SFM_READ, &info2);
    if (!infile2) {
        qDebug() << "Error opening file" << input2;
        sf_close(infile1);
        return;
    }

    // 检查音频格式是否相同
    if (info1.channels != info2.channels || info1.samplerate != info2.samplerate) {
        qDebug() << "Error: Audio formats do not match";
        sf_close(infile1);
        sf_close(infile2);
        return;
    }

    // 创建输出音频文件
    SF_INFO outinfo = info1;
    SNDFILE* outfile = sf_open(output.toStdString().c_str(), SFM_WRITE, &outinfo);
    if (!outfile) {
        qDebug() << "Error creating output file" << output;
        sf_close(infile1);
        sf_close(infile2);
        return;
    }

    // 读取并合并音频数据
    const int BUFFER_SIZE = 1024;
    QVector<short> buffer1(BUFFER_SIZE * info1.channels);
    QVector<short> buffer2(BUFFER_SIZE * info2.channels);
    int framesRead1 = 0;
    int framesRead2 = 0;

    while ((framesRead1 = sf_read_short(infile1, buffer1.data(), BUFFER_SIZE)) > 0 &&
           (framesRead2 = sf_read_short(infile2, buffer2.data(), BUFFER_SIZE)) > 0) {
        // 确保读取的帧数相同
        int framesToWrite = qMin(framesRead1, framesRead2);

        // 合并音频数据并写入输出文件
        sf_write_short(outfile, buffer1.data(), framesToWrite * info1.channels);
        sf_write_short(outfile, buffer2.data(), framesToWrite * info2.channels);
    }

    // 关闭文件
    sf_close(infile1);
    sf_close(infile2);
    sf_close(outfile);

    qDebug() << "Audio files merged successfully";
}

bool ClientSocketItem::deleteFile(const QString& filePath){
    QFile file(filePath);
    if (!file.exists()) {
        qDebug() << "File" << filePath << "does not exist. deleteFile";
        return false;
    }

    if (!file.remove()) {
        qDebug() << "Error deleting file " << filePath;
        return false;
    }

    qDebug() << "File" << filePath << "deleted successfully.";
    return true;
}
#endif

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
#if RECODE
    //如果从接听状态改变到空闲状态了，说明通话结束了
    if(this->status == ANSWERINGSTATUS && status == AVAILABLE){

        // 关闭文件
        if(file_dial && file_answer){
            sf_close(file_dial);
            sf_close(file_answer);
            QString outputFilePath = "data/"+getSocket()->peerAddress().toString().replace("::ffff:", "")+
                "_to_"+targetClientItem->getSocket()->peerAddress().toString().replace("::ffff:", "")+
                 beginTime.toString("yyyy-MM-dd_hh-mm-ss")+".wav";
            mergeAudio(filePath_answer, filePath_dial,outputFilePath);
            //删除临时文件
            deleteFile(filePath_answer);
            deleteFile(filePath_dial);
            file_dial = nullptr;
            file_answer = nullptr;
        }else{
            qDebug()<<"dile_dial or file_answer is null"<<file_answer<<" "<<file_dial;
        }
    }
#endif
    this->status = status;
    emit statusChanged(this->clientSocket->peerAddress().toString().replace("::ffff:", ""),status);
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

//删除不必要的0
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

//主动挂电话
void ClientSocketItem::hangUPTheCall(){

    AGREEANSWERING = 0;

    //保存历史记录
    QString dialerIP;
    QString answerIP;

    if(this->status == DIALSTATUS){
        dialerIP = getSocket()->peerAddress().toString().replace("::ffff:","");
        answerIP = targetClientItem->getSocket()->peerAddress().toString().replace("::ffff:","");
    }else{
        dialerIP = targetClientItem->getSocket()->peerAddress().toString().replace("::ffff:","");
        answerIP = getSocket()->peerAddress().toString().replace("::ffff:","");
    }

    emit requestSaveHistory(dialerIP,answerIP,(int)beginTime.secsTo(QDateTime::currentDateTime()),beginTime);

    //返回应答信号
    //if(clientSocket->state()  != QAbstractSocket::UnconnectedState)
        rebackACKFrame(4);

    //告知对方设备
    emit requestToSend(*hangupFrame,hangupFrame->count());

    //通知Server 转发至UI
    emit hangUp(this);
        //qDebug()<<"123";

    emit requestToHangup();
        //qDebug()<<"1234";
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
    //qDebug()<<"12345";
}

void ClientSocketItem::hangUPed(){//被动挂电话
    AGREEANSWERING = 0;
    setStatus(AVAILABLE);
    //qDebug()<<"hangup123";
    setTatgetClientItem(nullptr);
}

/*
*开始等待对方应答
*/
void ClientSocketItem::beginWaitANSER(){
    if(AGREEANSWERING == 0){
        emit callingStatusChange(1);
        AGREEANSWERING = 1;//接听未应答状态
        // 启动定时器
        timer->start();
    }

}

void ClientSocketItem::setBeginTime(const QDateTime& time){
    this->beginTime  = time;
}

QDateTime ClientSocketItem::getBeginTime(){
    return beginTime;
}


/*
@参数：STATUS 取值：true false；
      当STATUS取值为true时：监听当前socket
      当STATUS取值为true时：停止监听当前socket
*/
void ClientSocketItem::setMonitor(bool STATUS){
    if(STATUS){
        this->monitor = true;
    }else{
        this->monitor = false;
    }
}

void ClientSocketItem::onLine(){
    clientSocket->read(1);
    legality = true;
    rebackACKFrame(1);
    //检测是否有相同IP
    Server::searchSameIP(this);

    //将当前用户添加至 在线用户组
    Server::addOnlineClient(this);
    Server::showOnlineClients();
    emit onlineClientSingal(this->clientSocket->peerAddress().toString().replace("::ffff:", ""),this->status);
}

void ClientSocketItem::offLine(){
    hangUPTheCall();
    if(this->status != AVAILABLE)
        emit hangUp(this);
    emit offLineSingal(clientSocket->peerAddress().toString().replace("::ffff:", ""));

    this->legality = false;

    Server::deleteOnlieClient(this);
}

#if RECODE
bool ClientSocketItem::beginRecording(){
    //初始化录音文件
    fileInfo.channels = CHANNELS;             // 声道数
    fileInfo.samplerate = SAMPLERATE;       // 采样率
    fileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;  // WAV文件格式 16位色深

    QString timeStr = beginTime.toString("yyyy-MM-dd_hh-mm-ss");//格式化时间

    if(targetClientItem){
        filePath_dial = "data/temp/"+this->clientSocket->peerAddress().toString().replace("::ffff:", "")+"_"+timeStr+".wav";
        filePath_answer = "data/temp/"+this->targetClientItem->getSocket()->peerAddress().toString().replace("::ffff:", "")+"_"+timeStr+".wav";
    }else{
        return false;
    }

    QByteArray filePathBytes1 = filePath_dial.toUtf8();
    QByteArray filePathBytes2 = filePath_answer.toUtf8();

    const char* filePathStr1 = filePathBytes1.constData();
    const char* filePathStr2 = filePathBytes2.constData();

    this->file_dial = sf_open(filePathStr1, SFM_WRITE, &fileInfo);
    this->file_answer = sf_open(filePathStr2, SFM_WRITE, &fileInfo);

    if (!file_dial) {
        qDebug()<<"无法创建录音文件_file_dial beginRecording";
        return false;
    }
    if (!file_answer) {
        qDebug()<<"无法创建录音文件_file_answer beginRecording";
        return false;
    }

    return true;
}
#endif

/*
 * 处理IP数据帧后面的数据部分
 * 如果接收到正确格式的数据，返回true，否则返回false
 * 当前函数可能存在问题，当tcp流中不足15字节时的处理可能会出现问题
 *
*/
bool ClientSocketItem::handleIPDATA(){
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
        //clientSocket->read(15);
        //rebackACKFrame(1);
        //willreceive = HEADER;
        //willReceiveLength = 0;
        return false;
    }
    willreceive = HEADER;
    willReceiveLength = 0;
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
#if DEBUG
            printfLog(clientSocket->peerAddress().toString().replace("::ffff:","")+" 下线 ");
#endif
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

#if MONITOR

#endif

#if RECODE
    //录音操作（这里录的是接听方的声音）
    if(this->status == DIALSTATUS && !sf_write(data,file_answer)){
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
