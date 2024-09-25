#include "recoder.h"

Recoder::Recoder():thread(new QThread)  {
    moveToThread(thread);
}

bool readWavFile(const QString& filePath, QByteArray& audioData, int& sampleRate) {
    SF_INFO info;
    info.format = 0;
    SNDFILE* file = sf_open(filePath.toStdString().c_str(), SFM_READ, &info);
    if (!file) {
        qCritical() << "Failed to open WAV file:" << filePath;
        return false;
    }

    int channels = info.channels;
    int frames = info.frames;
    sampleRate = info.samplerate;
    int sampleCount = channels * frames;

    audioData.resize(sampleCount * sizeof(qint16));
    sf_count_t bytesRead = sf_read_short(file, reinterpret_cast<qint16*>(audioData.data()), sampleCount);
    sf_close(file);

    if (bytesRead != sampleCount) {
        qCritical() << "Failed to read audio data from WAV file:" << filePath;
        return false;
    }

    return true;
}

bool writeWavFile(const QString& filePath, const QByteArray& audioData, int sampleRate) {
    SF_INFO info;
    info.channels = 2;
    info.samplerate = sampleRate;
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    SNDFILE* file = sf_open(filePath.toStdString().c_str(), SFM_WRITE, &info);
    if (!file) {
        qCritical() << "Failed to create WAV file:" << filePath;
        return false;
    }

    sf_count_t bytesWritten = sf_write_short(file, reinterpret_cast<const qint16*>(audioData.constData()), audioData.size() / sizeof(qint16));
    sf_close(file);

    if (bytesWritten != audioData.size() / sizeof(qint16)) {
        qCritical() << "Failed to write audio data to WAV file:" << filePath;
        return false;
    }

    return true;
}

void mixAudioData(const QByteArray& audioData1, const QByteArray& audioData2, QByteArray& mixedAudioData) {
    int sampleCount1 = audioData1.size() / sizeof(qint16);
    int sampleCount2 = audioData2.size() / sizeof(qint16);
    int sampleCountMax = qMax(sampleCount1, sampleCount2);

    const qint16* samples1 = reinterpret_cast<const qint16*>(audioData1.data());
    const qint16* samples2 = reinterpret_cast<const qint16*>(audioData2.data());
    mixedAudioData.resize(sampleCountMax * sizeof(qint16));
    qint16* mixedSamples = reinterpret_cast<qint16*>(mixedAudioData.data());

    for (int i = 0; i < sampleCountMax; ++i) {
        int sample1 = (i < sampleCount1) ? samples1[i] : 0;
        int sample2 = (i < sampleCount2) ? samples2[i] : 0;
        mixedSamples[i] = std::clamp(sample1 + sample2, static_cast<int>(std::numeric_limits<qint16>::min()), static_cast<int>(std::numeric_limits<qint16>::max()));
    }
}

void Recoder::mergeAudio(const QString& input1, const QString& input2, const QString& output) {
    QByteArray audioData1, audioData2, mixedAudioData;
    int sampleRate = 6000;

    if (!readWavFile(input1, audioData1, sampleRate)) {
        qCritical() << "Failed to read WAV file:" << input1;
        return;
    }

    if (!readWavFile(input2, audioData2, sampleRate)) {
        qCritical() << "Failed to read WAV file:" << input2;
        return;
    }

    mixAudioData(audioData1, audioData2, mixedAudioData);

    if (!writeWavFile(output, mixedAudioData, sampleRate)) {
        qCritical() << "Failed to write WAV file:" << output;
        return;
    }

    qDebug() << "WAV files merged successfully into:" << output;
}

bool Recoder::sf_write(const QByteArray buffer, SNDFILE* file){
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
