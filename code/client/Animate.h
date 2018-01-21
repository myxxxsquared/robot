//
// Created by Shikang on 2018/1/18.
//

#ifndef FRITZ_CONTROL_ANIMATE_H
#define FRITZ_CONTROL_ANIMATE_H

#include "Robot.h"
#include "StateSeries.h"
#include <mutex>
#include <queue>
#include <vector>

#define IDLE_STATE_LEN  4

const std::vector<RobotState> IdleStateLst = {
        CenterState, RightState,
        CenterState, LeftState };

StateSeries GetNodSeries();

StateSeries GetSpeakSeries();

class Animate{
private:
    volatile bool abort;  // whether the robot should run.
    volatile bool work;  // whether automatically add new items into queue.
    std::mutex *p_state_mtx;
    Serial * p_serial;
    Robot * p_robot;
    int IdleState;
    RobotState next_state;
    std::thread *p_work_thread;
    StateSeries last_action;

    void StartWork();
    void DoWork();
public:
    std::queue<StateSeries> q_state_series;
    std::queue<RobotState> q_robot_state;
//    Animate();
    explicit Animate(std::string port_name);
    explicit Animate(Robot * r);
    explicit Animate(Serial* s);
    Animate(const Animate& a);
    void Abort();
    void Init();
    void ClearWork();
    void SetIdle();

    void TransmitState();
};


#endif //FRITZ_CONTROL_ANIMATE_H
