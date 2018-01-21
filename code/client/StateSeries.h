//
// Created by Shikang on 2018/1/21.
//

#ifndef FRITZ_CONTROL_STATESERIES_H
#define FRITZ_CONTROL_STATESERIES_H

#include "Robot.h"
#include <vector>

class StateSeries{
public:
    std::vector<RobotState> v_robot_state;
    StateSeries()= default;
//    static StateSeries GetStateIdle();
//    static StateSeries GetStateSpeak();
//    static StateSeries GetStateLeft();
//    static StateSeries GetStateRight();
//    static const RobotState VoidState = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0);
//
//    static const RobotState SpeakAAAH = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .50, .50, 0, -1, -1, 0, 0);
//    static const RobotState SpeakOH = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .10, .90, .50, -1, -1, 0, 0);
//    static const RobotState SpeakAA = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .10, .90, .20, -1, -1, 0, 0);
//    static const RobotState SpeakI = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .30, .70, 1.0, -1, -1, 0, 0);
//    static const RobotState SpeakLAA = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .70, .30, .20, -1, -1, 0, 0);
//    static const RobotState SpeakSS = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .70, .30, .20, -1, -1, 0, 0);
//    static const RobotState SpeakSSS = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .20, .80, .90, -1, -1, 0, 0);
//    static const RobotState SpeakEEE = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .10, .90, .30, -1, -1, 0, 0);
//    static const RobotState SpeakOH_ = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .50, .50, .10, -1, -1, 0, 0);
//    static const RobotState SpeakOOOH = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .20, .80, .10, -1, -1, 0, 0);
//    static const RobotState SpeakFUH = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .50, .50, .90, -1, -1, 0, 0);
//    static const RobotState SpeakMMM = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .40, .60, 1.0, -1, -1, 0, 0);
//    static const RobotState Speak___ = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .50, .50, 1.0, -1, -1, 0, 0);
//
//    static const RobotState Nod_S0 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .60, -1, 0, 0);
//    static const RobotState Nod_S1 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .90, -1, 0, 0);
//    static const RobotState Nod_S2 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .60, -1, 0, 0);
//    static const RobotState Nod_S3 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .30, -1, 0, 0);
//    static const RobotState Nod_S4 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .50, -1, 0, 0);
//
//    static const RobotState CenterState = RobotState(.40, .40, .50, .50, .50, .50, .50, .50, .50, .50, .0, -1, .50, 0, 0);
//    static const RobotState RightState = RobotState(.40, .40, .50, .50, .40, .45, .50, .50, .60, .60, .50, -1, .70, 0, 0);
//    static const RobotState LeftState = RobotState(.40, .40, .50, .50, .40, .55, .50, .50, .40, .40, .50, -1, .30, 0, 0);

};
const RobotState VoidState = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0);

const RobotState SpeakAAAH = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .50, .50, 0, -1, -1, 0, 0);
const RobotState SpeakOH = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .10, .90, .50, -1, -1, 0, 0);
const RobotState SpeakAA = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .10, .90, .20, -1, -1, 0, 0);
const RobotState SpeakI = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .30, .70, 1.0, -1, -1, 0, 0);
const RobotState SpeakLAA = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .70, .30, .20, -1, -1, 0, 0);
const RobotState SpeakSS = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .70, .30, .20, -1, -1, 0, 0);
const RobotState SpeakSSS = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .20, .80, .90, -1, -1, 0, 0);
const RobotState SpeakEEE = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .10, .90, .30, -1, -1, 0, 0);
const RobotState SpeakOH_ = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .50, .50, .10, -1, -1, 0, 0);
const RobotState SpeakOOOH = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .20, .80, .10, -1, -1, 0, 0);
const RobotState SpeakFUH = RobotState(-1, -1, -1, -1, -1, -1, .90, .90, .50, .50, .90, -1, -1, 0, 0);
const RobotState SpeakMMM = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .40, .60, 1.0, -1, -1, 0, 0);
const RobotState Speak___ = RobotState(-1, -1, -1, -1, -1, -1, .70, .70, .50, .50, 1.0, -1, -1, 0, 0);

const RobotState Nod_S0 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .60, -1, 0, 0);
const RobotState Nod_S1 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .90, -1, 0, 0);
const RobotState Nod_S2 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .60, -1, 0, 0);
const RobotState Nod_S3 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .30, -1, 0, 0);
const RobotState Nod_S4 = RobotState(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, .50, -1, 0, 0);

const RobotState CenterState = RobotState(.40, .40, .50, .50, .50, .50, .50, .50, .50, .50, .0, -1, .50, 0, 0);
const RobotState RightState = RobotState(.40, .40, .50, .50, .40, .45, .50, .50, .60, .60, .50, -1, .70, 0, 0);
const RobotState LeftState = RobotState(.40, .40, .50, .50, .40, .55, .50, .50, .40, .40, .50, -1, .30, 0, 0);
#endif //FRITZ_CONTROL_STATESERIES_H
