//
// Created by Shikang on 2018/1/18.
//

#ifndef FRITZ_CONTROL_ANIMATE_H
#define FRITZ_CONTROL_ANIMATE_H

#include "Robot.h"
#include <mutex>
#include <queue>
#include <vector>

#define IDLE_STATE_LEN = 4


class StateSeries{
public:
    std::vector<RobotState> v_robot_state;
    static StateSeries GetStateIdle();
    static StateSeries GetStateSpeak();
    static StateSeries GetStateLeft();
    static StateSeries GetStateRight();
};


const RobotState CenterState(.40, .40, .50, .50, .50, .50, .50, .50, .50, .50, .0, -1, .50, 0, 0);
const RobotState RightState(.40, .40, .50, .50, .40, .45, .50, .50, .60, .60, .50, -1, .70, 0, 0);
const RobotState LeftState(.40, .40, .50, .50, .40, .55, .50, .50, .40, .40, .50, -1, .30, 0, 0);
const std::vector<RobotState> IdleStateLst = { CenterState, RightState, CenterState, LeftState };

class Animate{
private:
    volatile bool abort;
    volatile bool work;
    std::mutex *p_state_mtx;
    Serial * p_serial;
    Robot * p_robot;
    int IdleState;
    RobotState next_state;
    std::thread *p_work_thread;


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
    void StartWork();
    void DoWork();

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
