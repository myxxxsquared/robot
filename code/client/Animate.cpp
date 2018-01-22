//
// Created by Shikang on 2018/1/18.
//

#include "Animate.h"
#include <iostream>
StateSeries GetNodSeries(){
    StateSeries NodSS;
    NodSS.v_robot_state.push_back(Nod_S0);
    NodSS.v_robot_state.push_back(Nod_S1);
    NodSS.v_robot_state.push_back(Nod_S2);
    NodSS.v_robot_state.push_back(Nod_S3);
    NodSS.v_robot_state.push_back(Nod_S4);
    return NodSS;
}

StateSeries GetSpeakSeries(){
    StateSeries SpeakSS;
    SpeakSS.v_robot_state.push_back(CenterState);
    SpeakSS.v_robot_state.push_back(Speak___);
    SpeakSS.v_robot_state.push_back(VoidState);
    SpeakSS.v_robot_state.push_back(VoidState);
    SpeakSS.v_robot_state.push_back(SpeakFUH);
    return SpeakSS;
}

StateSeries GetRightSeries(){
    StateSeries RSS;
    RSS.v_robot_state.push_back(RightState);
    RSS.v_robot_state.push_back(VoidState);
    RSS.v_robot_state.push_back(VoidState);
    RSS.v_robot_state.push_back(VoidState);
    RSS.v_robot_state.push_back(VoidState);
    return RSS;
}

StateSeries GetLeftSeries(){
    StateSeries RSS;
    RSS.v_robot_state.push_back(LeftState);
    RSS.v_robot_state.push_back(VoidState);
    RSS.v_robot_state.push_back(VoidState);
    RSS.v_robot_state.push_back(VoidState);
    RSS.v_robot_state.push_back(VoidState);
    return RSS;
}

Animate::Animate(Robot* r){
    p_serial = nullptr;
    p_robot = r;
    abort = false;
    work = false;
    q_robot_state = std::queue<RobotState>();
    p_state_mtx = new std::mutex();
}

Animate::Animate(Serial* s) {
    p_serial = s;
    p_robot = new Robot(s);
    abort = false;
    work = false;
    q_robot_state = std::queue<RobotState>();
    p_state_mtx = new std::mutex();
}

Animate::Animate(std::string str) {
    p_serial = new Serial();
    p_serial->arduino_port_name = str;
    p_robot = new Robot(p_serial);
    abort = false;
    work = false;
    q_robot_state = std::queue<RobotState>();
    p_state_mtx = new std::mutex();
}

Animate::Animate(const Animate &a) {
    p_serial = a.p_serial;
    p_robot = a.p_robot;
    abort = a.abort;
    work = a.work;
    p_state_mtx = a.p_state_mtx;
    q_robot_state = a.q_robot_state;
    q_state_series = a.q_state_series;
}

void Animate::Abort(){
    p_state_mtx->lock();
    work = false;
    abort = true;
    q_robot_state = std::queue<RobotState>();
    p_state_mtx->unlock();
    std::cout<<"--- Robot Terminated."<<std::endl;
}

void Animate::Init(){
    p_state_mtx->lock();
    work = false;
    abort = false;
    p_state_mtx->unlock();
    IdleState = 0;

    p_robot->SetCentre();
    p_work_thread = new std::thread(&Animate::StartWork, this);
//    StartWork();
}

void Animate::TransmitState(){
    if(q_state_series.empty()) return;
    StateSeries s = q_state_series.front();
    q_state_series.pop();
    last_action = s;
    for(auto iter = s.v_robot_state.begin(); iter!=s.v_robot_state.end();iter++){
        q_robot_state.push(*iter);
    }
}

void Animate::StartWork(){
//    Init();
    p_robot->InitRobot();
    DoWork();
}

void Animate::DoWork(){
    while(true){
        p_state_mtx->lock();
        if (abort) return;
        TransmitState();
        if(q_robot_state.empty()){
            if (not work){  // Idle state
                SetIdle();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            else{  // add last action into queue
                q_state_series.push(last_action);
                p_state_mtx->unlock();
//                std::cout<<"+++ last continue"<<std::endl;
                continue;
            }
        }
        else{
            next_state = q_robot_state.front();
            q_robot_state.pop();
        }
        p_state_mtx->unlock();

        p_robot->SetState(next_state);
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); // TODO: check
    }
}

void Animate::SetContinuousWork(StateSeries ss){
    p_state_mtx->lock();
    work = true;
    q_state_series.push(ss);
    p_state_mtx->unlock();
}

void Animate::ClearWork(){
    p_state_mtx->lock();
    work = false;
    q_state_series = std::queue<StateSeries>();
    q_robot_state = std::queue<RobotState>();
    p_state_mtx->unlock();
}

void Animate::SetIdle(){
    IdleState = (IdleState + 1) % IDLE_STATE_LEN;
    next_state = IdleStateLst[IdleState];
}
