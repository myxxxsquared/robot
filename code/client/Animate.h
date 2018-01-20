//
// Created by Shikang on 2018/1/18.
//

#ifndef FRITZ_CONTROL_ANIMATE_H
#define FRITZ_CONTROL_ANIMATE_H

#include "Robot.h"
#include <mutex>

class Animate
{
public:
    explicit Animate(Serial *serial);
    void requestWork();
    void abort();
    void SpeakMessage(std::string msg);

    void doWork();
    void doWorkOld();

private:
    bool _working;
    bool _abort;
    Serial *_serial;
//    QMutex mutex;
    std::mutex mutex;

    std::string text;

//    signals:
    void workRequested();
    void animate();
    void finished();
    void done();

//public slots:
//    void doWork();
//    void doWorkOld();
};



#endif //FRITZ_CONTROL_ANIMATE_H
