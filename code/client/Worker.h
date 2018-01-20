////
//// Created by Shikang on 2018/1/18.
////
//
//#ifndef FRITZ_CONTROL_WORKER_H
//#define FRITZ_CONTROL_WORKER_H
//
//#include <thread>
//#include <mutex>
//
//class Worker
//{
//public:
//    explicit Worker(QObject *parent = 0);
//    void requestWork();
//    void abort();
//
//private:
//    bool _working;
//    bool _abort;
//    std::mutex mutex;
//
//    signals:
//    void workRequested();
//    void valueChanged(int);
//    void finished();
//
//public slots:
//    void doWork();
//};
//
//#endif //FRITZ_CONTROL_WORKER_H
