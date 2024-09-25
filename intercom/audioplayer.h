
#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "config.h"
#include <QObject>
#include <QThread>
#include <QBuffer>
#include <QDebug>
#include <SFML/Audio.hpp>

#if MONITOR
class AudioPlayer:public QObject
{
    Q_OBJECT
public:
    AudioPlayer(int channels,int samplerate);



public slots:
    void playAudio(const QByteArray audioData);
    void Test();

private:
    sf::SoundBuffer soundBuffer;
    sf::Sound sound;
    int channels,samplerate;

    QThread *thread;
};
#endif

#endif // AUDIOPLAYER_H
