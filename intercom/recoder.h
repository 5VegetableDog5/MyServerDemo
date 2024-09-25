

#ifndef RECODER_H
#define RECODER_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QFile>

#include "sndfile.h"
class Recoder : public QObject
{
    Q_OBJECT
public:
    Recoder();

    //合并音频
    void mergeAudio(const QString& input1, const QString& input2, const QString& output);

    bool static sf_write(const QByteArray buffer, SNDFILE* file);

private:
    QThread *thread;
};

#endif // RECODER_H

