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
#include <thread>

#define IDLE_STATE_LEN  4



//const std::vector<RobotState> IdleStateLst = {
//        StateSeries::CenterState, StateSeries::RightState,
//        StateSeries::CenterState, StateSeries::LeftState };
//
//StateSeries GetNodSeries(){
//    StateSeries NodSS;
//    NodSS.v_robot_state.push_back(StateSeries::Nod_S0);
//    NodSS.v_robot_state.push_back(StateSeries::Nod_S1);
//    NodSS.v_robot_state.push_back(StateSeries::Nod_S2);
//    NodSS.v_robot_state.push_back(StateSeries::Nod_S3);
//    NodSS.v_robot_state.push_back(StateSeries::Nod_S4);
//    return NodSS;
//}
//
//StateSeries GetSpeakSeries(){
//    StateSeries SpeakSS;
//    SpeakSS.v_robot_state.push_back(StateSeries::CenterState);
//    SpeakSS.v_robot_state.push_back(StateSeries::Speak___);
//    SpeakSS.v_robot_state.push_back(StateSeries::VoidState);
//    SpeakSS.v_robot_state.push_back(StateSeries::VoidState);
//    SpeakSS.v_robot_state.push_back(StateSeries::SpeakFUH);
//    return SpeakSS;
//}
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
    void SetContinuousWork(StateSeries ss);
    void ClearWork();
    void SetIdle();

    void TransmitState();
};

//RobotState(const float r_leftHorizontalEye, const float r_rightHorizontalEye, const float r_leftVerticalEye,
//           const float r_rightVerticalEye, const float r_leftEyebrow, const float r_rightEyebrow,
//           const float r_leftEyelid, const float r_rightEyelid, const float r_leftLip,
//           const float r_rightLip, const float r_jaw, const float r_neckTilt, const float r_neckTwist,
//           const int r_position, const int r_triggerPosition)
//        ;
//SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 60 /* 50 rightHorizontalEye*/,
//10 /* 50 rightVerticalEye*/, 50 /*leftEyebrow*/, 50 /*rightEyebrow*/, 50 /*rightEyelid*/,
//50 /*leftEyelid*/, 50 /*leftLip*/, 50 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
//void Robot::SetLeft() {
//    SetState(40, -1, 40, -1, 30, 70, 100, 100, 50, 50, 50, -1, 10);
//}
//
//void Robot::SetCentre() {
//    SetState(40, -1, 40, -1, 50, 50, 100, 100, 50, 50, 50, -1, 50);
//}
//
//void Robot::SetRight() {
//    SetState(40, -1, 40, -1, 30, 70, 100, 100, 50, 50, 50, -1, 90);
//}



#endif //FRITZ_CONTROL_ANIMATE_H
