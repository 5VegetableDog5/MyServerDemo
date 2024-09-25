#include "audioplayer.h"
#if MONITOR
AudioPlayer::AudioPlayer(int channels,int samplerate):thread(new QThread) {
    this->channels = channels;
    this->samplerate = samplerate;

    //moveToThread(thread);
}

void AudioPlayer::playAudio(const QByteArray soundData){
    QByteArray soundData2(soundData);
    //qDebug()<<"playAudio Singal";
    if(soundData.size() == 0){
        qDebug()<<"音频播放失败！soundData 为 0 ！";
        return;
    }
    // 从QByteArray中获取音频数据
    const qint16* data = reinterpret_cast<const qint16*>(soundData.constData());
    std::size_t dataSize = soundData.size() / sizeof(qint16);


    //qDebug()<< dataSize;
    // 从文件中加载音频数据
    if (soundBuffer.loadFromSamples(data, dataSize ,channels,samplerate)) {
        // 将音频数据设置到音频对象中
        sound.setBuffer(soundBuffer);

        // 播放音频
        sound.play();

        // 等待音频播放完毕
        //while (sound.getStatus() == sf::Sound::Playing) {}
    }else{
        qDebug()<<"AudioPlayer-playAudio :加载音频数据失败！ ";
    }
}

void AudioPlayer::Test(){
    qDebug()<<"\n\n\n\n\n\n\n\n\n\n\n\n\n----------------------------\n";
}
#endif
