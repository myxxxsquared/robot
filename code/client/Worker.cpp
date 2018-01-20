////
//// Created by Shikang on 2018/1/18.
////
//
//#include "Worker.h"
//
//Worker::Worker(QObject *parent)
//{
//    _working = false;
//    _abort = false;
//}
//
//void Worker::doWork()
//{
//    bool running = true;
//    while(running)
//    {
//        for (int i = 0; i < 100; i = i + 5)
//        {
//            mutex.lock();
//            bool abort = _abort;
//            mutex.unlock();
//
//            if (abort)
//            {
//                running = false;
//                break;
//            }
//
//            QEventLoop loop;
//            QTimer::singleShot(250, &loop, SLOT(quit()));
//            loop.exec();
//
//            emit valueChanged(i);
//        }
//
//        for (int i = 100; i > 0; i = i - 5)
//        {
//            mutex.lock();
//            bool abort = _abort;
//            mutex.unlock();
//
//            if (abort)
//            {
//                running = false;
//                break;
//            }
//
//            QEventLoop loop;
//            QTimer::singleShot(250, &loop, SLOT(quit()));
//            loop.exec();
//
//            emit valueChanged(i);
//        }
//
//    }
//    mutex.lock();
//    _working = false;
//    mutex.unlock();
//
//    emit finished();
//}
//
//void Worker::requestWork()
//{
//    mutex.lock();
//    _working = true;
//    _abort = false;
//    mutex.unlock();
//
//    emit workRequested();
//}
//
//void Worker::abort()
//{
//    if (_working) {
//        mutex.lock();
//        _working = false;
//        _abort = true;
//        mutex.unlock();
//        qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
//    }
//}
