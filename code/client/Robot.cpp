//
// Created by Shikang on 2018/1/18.
//

#include <iostream>
#include <chrono>
#include <thread>
#include "Robot.h"

CalibrationData::CalibrationData() = default;

int convFloatStateValInt(int min, int max, float f_val){
//    return (int)(min + (max-min) * f_val);
    if (f_val == -1) return -1;
    if (f_val < 0) return -1;
    return (int)(f_val * 100);  // just conv float to percentage
}

Robot::Robot(Serial *serial) {
    //serial = new Serial();
    this->serial = serial;

    const int minPin = 2;
    const int maxPin = 19;

    float irValue = 1000.0f;
    float sonarValue = 1000.0f;

    cd = new CalibrationData();  // TODO: this is not yet used

    leftHorizontalEyeMin = -400;
    leftHorizontalEyeMax = 100;
    leftHorizontalEyePin = 4;
    leftVerticalEyeMin = -300;
    leftVerticalEyeMax = 300;
    leftVerticalEyePin = 6;  // 5
    rightHorizontalEyeMin = -400;
    rightHorizontalEyeMax = 200;
    rightHorizontalEyePin = 9;
    rightVerticalEyeMin = -400;
    rightVerticalEyeMax = 200;
    rightVerticalEyePin = 11;  // 10
    leftLipMin = -200;
    leftLipMax = 500;
    leftLipPin = 10;  // 6
    rightLipMin = -500;
    rightLipMax = 100;
    rightLipPin = 5;  // 11
    jawMin = -300;
    jawMax = -80;
    jawPin = 14;  // A0; 14
    neckTiltMin = -700;
    neckTiltMax = -450;
    neckTiltPin = 13;
    neckTwistMin = -1000;
    neckTwistMax = -500;
    neckTwistPin = 12;
    leftEyebrowMin = -180;
    leftEyebrowMax = 180;
    leftEyebrowPin = 2;
    rightEyebrowMin = -180;
    rightEyebrowMax = 180;
    rightEyebrowPin = 7;
    leftEyelidMin = -175;  // -150
    leftEyelidMax = 400;
    leftEyelidPin = 3;  // 8
    rightEyelidMin = -450;
    rightEyelidMax = 10;
    rightEyelidPin = 8;  // 3

    sonarOutPin = 15;
    sonarInPin = 16;

    // Prime the distance sensor
    // GetSonar();
//    this->serial->Open();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

Robot::~Robot() {
    serial->Close();
    delete cd;
}

void Robot::InitRobot(){
    this->serial->Open();
    SetCentre();
    std::cout<<"--- Robot Inited."<<std::endl;
}


void Robot::Reset() {
    RobotState * initState = new RobotState();
}


double Robot::GetSonar() {
    serial->SendCommand(ARDUINO_GET_SONAR, sonarOutPin, sonarInPin);
    sonarValue = serial->GetSonar();

    return sonarValue;
}

void Robot::SetMouth(std::string shape) {
    if (shape == "aaah") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 90);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 90);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 50);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 50);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 0);
    } else if (shape == "oh") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 70);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 70);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 10);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 90);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 50);
    } else if (shape == "aa") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 70);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 70);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 10);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 90);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 20);
    } else if (shape == "i") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 70);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 70);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 30);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 70);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 100);
    } else if (shape == "laa") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 90);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 90);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 70);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 30);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 20);
    } else if (shape == "sss") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 70);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 70);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 20);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 80);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 90);
    } else if (shape == "eee") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 70);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 70);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 10);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 90);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 30);
    } else if (shape == "oh") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 90);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 90);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 50);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 50);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 10);
    } else if (shape == "oooh") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 90);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 90);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 20);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 80);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 0);
    } else if (shape == "fuh") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 90);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 90);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 50);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 50);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 90);
    } else if (shape == "mmm") {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 70);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 70);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 40);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 60);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 100);
    } else {
        serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, 70);
        serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, 70);
        serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, 50);
        serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, 50);
        serial->DoTest(Checked, jawMin, jawMax, jawPin, 100);
    }
}

void Robot::SetState(int n_leftHorizontalEye, int n_leftVerticalEye, int n_rightHorizontalEye, int n_rightVerticalEye,
                     int n_leftEyebrow, int n_rightEyebrow, int n_rightEyelid, int n_leftEyelid, int n_leftLip,
                     int n_rightLip, int n_jaw, int n_neckTilt, int n_neckTwist) {
    if (n_leftEyebrow != -1) serial->DoTest(Checked, leftEyebrowMin, leftEyebrowMax, leftEyebrowPin, n_leftEyebrow);
    if (n_rightEyebrow != -1)
        serial->DoTest(Checked, rightEyebrowMin, rightEyebrowMax, rightEyebrowPin, n_rightEyebrow);
    if (n_leftEyelid != -1) serial->DoTest(Checked, leftEyelidMin, leftEyelidMax, leftEyelidPin, n_leftEyelid);
    if (n_rightEyelid != -1) serial->DoTest(Checked, rightEyelidMin, rightEyelidMax, rightEyelidPin, n_rightEyelid);
    if (n_leftHorizontalEye != -1)
        serial->DoTest(Checked, leftHorizontalEyeMin, leftHorizontalEyeMax, leftHorizontalEyePin, n_leftHorizontalEye);
    if (n_rightHorizontalEye != -1)
        serial->DoTest(Checked, rightHorizontalEyeMin, rightHorizontalEyeMax, rightHorizontalEyePin,
                       n_rightHorizontalEye);
    if (n_leftVerticalEye != -1)
        serial->DoTest(Checked, leftVerticalEyeMin, leftVerticalEyeMax, leftVerticalEyePin, n_leftVerticalEye);
    if (n_rightVerticalEye != -1)
        serial->DoTest(Checked, rightVerticalEyeMin, rightVerticalEyeMax, rightVerticalEyePin, n_rightVerticalEye);
    if (n_leftLip != -1) serial->DoTest(Checked, leftLipMin, leftLipMax, leftLipPin, n_leftLip);
    if (n_rightLip != -1) serial->DoTest(Checked, rightLipMin, rightLipMax, rightLipPin, n_rightLip);
    if (n_neckTwist != -1) serial->DoTest(Checked, neckTwistMin, neckTwistMax, neckTwistPin, n_neckTwist);
    if (n_neckTilt != -1) serial->DoTest(Checked, neckTiltMin, neckTiltMax, neckTiltPin, n_neckTilt);
    if (n_jaw != -1) serial->DoTest(Checked, jawMin, jawMax, jawPin, n_jaw);
}

void Robot::SetState(RobotState rs) {

    SetState(convFloatStateValInt(leftHorizontalEyeMin, leftHorizontalEyeMax, rs.leftHorizontalEye),
             convFloatStateValInt(leftVerticalEyeMin, leftVerticalEyeMax, rs.leftVerticalEye),
             convFloatStateValInt(rightHorizontalEyeMin, rightHorizontalEyeMax, rs.rightHorizontalEye),
             convFloatStateValInt(rightVerticalEyeMin, rightVerticalEyeMax, rs.rightVerticalEye),
             convFloatStateValInt(leftEyebrowMin, leftEyebrowMax, rs.leftEyebrow),
             convFloatStateValInt(rightEyebrowMin,rightEyebrowMax, rs.rightEyebrow),
             convFloatStateValInt(rightEyelidMin, rightEyelidMax, rs.rightEyelid),
             convFloatStateValInt(leftEyelidMin, leftEyelidMax, rs.leftEyelid),
             convFloatStateValInt(leftLipMin, leftLipMax, rs.leftLip),
             convFloatStateValInt(rightLipMin, rightLipMax, rs.rightLip),
             convFloatStateValInt(jawMin, jawMax, rs.jaw),
             convFloatStateValInt(neckTiltMin, neckTiltMax, rs.neckTilt),
             convFloatStateValInt(neckTwistMin, neckTwistMax, rs.neckTwist));
}

void Robot::SetExpression(std::string name) {
    if (name == "Afraid")
        SetState(50 /*leftHorizontalEye*/, 70 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 70 /*rightVerticalEye*/,
                 82 /*leftEyebrow*/, 78 /*rightEyebrow*/, 62 /*rightEyelid*/, 6 /*leftEyelid*/, 11 /*leftLip*/,
                 85 /*rightLip*/, 22 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Awkward")
        SetState(52 /*leftHorizontalEye*/, 48 /*leftVerticalEye*/, 52 /*rightHorizontalEye*/, 48 /*rightVerticalEye*/,
                 100 /*leftEyebrow*/, 100 /*rightEyebrow*/, 45 /*rightEyelid*/, 45 /*leftEyelid*/, 100 /*leftLip*/,
                 0 /*rightLip*/, 67 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Angry")
        SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 50 /*rightVerticalEye*/,
                 1 /*leftEyebrow*/, 9 /*rightEyebrow*/, 40 /*rightEyelid*/, 40 /*leftEyelid*/, 22 /*leftLip*/,
                 77 /*rightLip*/, 0 /*jaw*/, 33 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Disappointed")
        SetState(47 /*leftHorizontalEye*/, 78 /*leftVerticalEye*/, 47 /*rightHorizontalEye*/, 78 /*rightVerticalEye*/,
                 44 /*leftEyebrow*/, 43 /*rightEyebrow*/, 7 /*rightEyelid*/, 7 /*leftEyelid*/, 31 /*leftLip*/,
                 68 /*rightLip*/, 0 /*jaw*/, 17 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Happy")
        SetState(63 /*leftHorizontalEye*/, 56 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 56 /*rightVerticalEye*/,
                 76 /*leftEyebrow*/, 66 /*rightEyebrow*/, 40 /*rightEyelid*/, 40 /*leftEyelid*/, 5 /*leftLip*/,
                 95 /*rightLip*/, 10 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Neutral")
        SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 60 /* 50 rightHorizontalEye*/,
                 10 /* 50 rightVerticalEye*/, 50 /*leftEyebrow*/, 50 /*rightEyebrow*/, 50 /*rightEyelid*/,
                 50 /*leftEyelid*/, 50 /*leftLip*/, 50 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Sad")
        SetState(56 /*leftHorizontalEye*/, 64 /*leftVerticalEye*/, 56 /*rightHorizontalEye*/, 64 /*rightVerticalEye*/,
                 10 /*leftEyebrow*/, 90 /*rightEyebrow*/, 57 /*rightEyelid*/, 57 /*leftEyelid*/, 95 /*leftLip*/,
                 5 /*rightLip*/, 90 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Sinister")
        SetState(50 /*leftHorizontalEye*/, 44 /*leftVerticalEye*/, 5 /*rightHorizontalEye*/, 44 /*rightVerticalEye*/,
                 7 /*leftEyebrow*/, 6 /*rightEyebrow*/, 6 /*rightEyelid*/, 6 /*leftEyelid*/, 70 /*leftLip*/,
                 29 /*rightLip*/, 67 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Sleepy")
        SetState(50 /*leftHorizontalEye*/, 70 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 70 /*rightVerticalEye*/,
                 90 /*leftEyebrow*/, 10 /*rightEyebrow*/, 10 /*rightEyelid*/, 10 /*leftEyelid*/, 25 /*leftLip*/,
                 75 /*rightLip*/, 100 /*jaw*/, 30 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Smile")
        SetState(63 /*leftHorizontalEye*/, 56 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 56 /*rightVerticalEye*/,
                 59 /*leftEyebrow*/, 58 /*rightEyebrow*/, 7 /*rightEyelid*/, 7 /*leftEyelid*/, 100 /*leftLip*/,
                 0 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Sneaky")
        SetState(62 /*leftHorizontalEye*/, 30 /*leftVerticalEye*/, 62 /*rightHorizontalEye*/, 30 /*rightVerticalEye*/,
                 58 /*leftEyebrow*/, 51 /*rightEyebrow*/, 26 /*rightEyelid*/, 26 /*leftEyelid*/, 0 /*leftLip*/,
                 0 /*rightLip*/, 92/*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Sulk")
        SetState(37 /*leftHorizontalEye*/, 60 /*leftVerticalEye*/, 37 /*rightHorizontalEye*/, 61 /*rightVerticalEye*/,
                 33 /*leftEyebrow*/, 32 /*rightEyebrow*/, 76 /*rightEyelid*/, 76 /*leftEyelid*/, 17 /*leftLip*/,
                 36 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Surprised")
        SetState(50 /*leftHorizontalEye*/, 47 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 47 /*rightVerticalEye*/,
                 10 /*leftEyebrow*/, 90 /*rightEyebrow*/, 100 /*rightEyelid*/, 100 /*leftEyelid*/, 25 /*leftLip*/,
                 71 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Yelling")
        SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 50 /*rightVerticalEye*/,
                 10 /*leftEyebrow*/, 9 /*rightEyebrow*/, 40 /*rightEyelid*/, 4 /*leftEyelid*/, 22 /*leftLip*/,
                 77 /*rightLip*/, 100 /*jaw*/, 33 /*neckTilt*/, -1 /*neckTwist*/);
    else if (name == "Worried")
        SetState(51 /*leftHorizontalEye*/, 37 /*leftVerticalEye*/, 51 /*rightHorizontalEye*/, 37 /*rightVerticalEye*/,
                 80 /*leftEyebrow*/, 80 /*rightEyebrow*/, 7 /*rightEyelid*/, 7 /*leftEyelid*/, 18 /*leftLip*/,
                 81 /*rightLip*/, 50 /*jaw*/, 67 /*neckTilt*/, -1 /*neckTwist*/);
    else
        SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 60 /* 50 rightHorizontalEye*/,
                 10 /* 50 rightVerticalEye*/, 50 /*leftEyebrow*/, 50 /*rightEyebrow*/, 50 /*rightEyelid*/,
                 50 /*leftEyelid*/, 50 /*leftLip*/, 50 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);

}

void Robot::SetExpression(int e) {
    if (e == 0)
        SetState(50 /*leftHorizontalEye*/, 70 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 70 /*rightVerticalEye*/,
                 82 /*leftEyebrow*/, 78 /*rightEyebrow*/, 62 /*rightEyelid*/, 6 /*leftEyelid*/, 11 /*leftLip*/,
                 85 /*rightLip*/, 22 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 1)
        SetState(52 /*leftHorizontalEye*/, 48 /*leftVerticalEye*/, 52 /*rightHorizontalEye*/, 48 /*rightVerticalEye*/,
                 100 /*leftEyebrow*/, 100 /*rightEyebrow*/, 45 /*rightEyelid*/, 45 /*leftEyelid*/, 100 /*leftLip*/,
                 0 /*rightLip*/, 67 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 2)
        SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 50 /*rightVerticalEye*/,
                 1 /*leftEyebrow*/, 9 /*rightEyebrow*/, 40 /*rightEyelid*/, 40 /*leftEyelid*/, 22 /*leftLip*/,
                 77 /*rightLip*/, 0 /*jaw*/, 33 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 3)
        SetState(47 /*leftHorizontalEye*/, 78 /*leftVerticalEye*/, 47 /*rightHorizontalEye*/, 78 /*rightVerticalEye*/,
                 44 /*leftEyebrow*/, 43 /*rightEyebrow*/, 7 /*rightEyelid*/, 7 /*leftEyelid*/, 31 /*leftLip*/,
                 68 /*rightLip*/, 0 /*jaw*/, 17 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 4)
        SetState(63 /*leftHorizontalEye*/, 56 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 56 /*rightVerticalEye*/,
                 76 /*leftEyebrow*/, 66 /*rightEyebrow*/, 40 /*rightEyelid*/, 40 /*leftEyelid*/, 5 /*leftLip*/,
                 95 /*rightLip*/, 10 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 5)
        SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 60 /* 50 rightHorizontalEye*/,
                 10 /* 50 rightVerticalEye*/, 50 /*leftEyebrow*/, 50 /*rightEyebrow*/, 50 /*rightEyelid*/,
                 50 /*leftEyelid*/, 50 /*leftLip*/, 50 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 6)
        SetState(56 /*leftHorizontalEye*/, 64 /*leftVerticalEye*/, 56 /*rightHorizontalEye*/, 64 /*rightVerticalEye*/,
                 10 /*leftEyebrow*/, 90 /*rightEyebrow*/, 57 /*rightEyelid*/, 57 /*leftEyelid*/, 95 /*leftLip*/,
                 5 /*rightLip*/, 90 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 7)
        SetState(50 /*leftHorizontalEye*/, 44 /*leftVerticalEye*/, 5 /*rightHorizontalEye*/, 44 /*rightVerticalEye*/,
                 7 /*leftEyebrow*/, 6 /*rightEyebrow*/, 6 /*rightEyelid*/, 6 /*leftEyelid*/, 70 /*leftLip*/,
                 29 /*rightLip*/, 67 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 8)
        SetState(50 /*leftHorizontalEye*/, 70 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 70 /*rightVerticalEye*/,
                 90 /*leftEyebrow*/, 10 /*rightEyebrow*/, 10 /*rightEyelid*/, 10 /*leftEyelid*/, 25 /*leftLip*/,
                 75 /*rightLip*/, 100 /*jaw*/, 30 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 9)
        SetState(63 /*leftHorizontalEye*/, 56 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 56 /*rightVerticalEye*/,
                 59 /*leftEyebrow*/, 58 /*rightEyebrow*/, 7 /*rightEyelid*/, 7 /*leftEyelid*/, 100 /*leftLip*/,
                 0 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 10)
        SetState(62 /*leftHorizontalEye*/, 30 /*leftVerticalEye*/, 62 /*rightHorizontalEye*/, 30 /*rightVerticalEye*/,
                 58 /*leftEyebrow*/, 51 /*rightEyebrow*/, 26 /*rightEyelid*/, 26 /*leftEyelid*/, 0 /*leftLip*/,
                 0 /*rightLip*/, 92/*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 11)
        SetState(37 /*leftHorizontalEye*/, 60 /*leftVerticalEye*/, 37 /*rightHorizontalEye*/, 61 /*rightVerticalEye*/,
                 33 /*leftEyebrow*/, 32 /*rightEyebrow*/, 76 /*rightEyelid*/, 76 /*leftEyelid*/, 17 /*leftLip*/,
                 36 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 12)
        SetState(50 /*leftHorizontalEye*/, 47 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 47 /*rightVerticalEye*/,
                 10 /*leftEyebrow*/, 90 /*rightEyebrow*/, 100 /*rightEyelid*/, 100 /*leftEyelid*/, 25 /*leftLip*/,
                 71 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 13)
        SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 50 /*rightHorizontalEye*/, 50 /*rightVerticalEye*/,
                 10 /*leftEyebrow*/, 9 /*rightEyebrow*/, 40 /*rightEyelid*/, 4 /*leftEyelid*/, 22 /*leftLip*/,
                 77 /*rightLip*/, 100 /*jaw*/, 33 /*neckTilt*/, -1 /*neckTwist*/);
    else if (e == 14)
        SetState(51 /*leftHorizontalEye*/, 37 /*leftVerticalEye*/, 51 /*rightHorizontalEye*/, 37 /*rightVerticalEye*/,
                 80 /*leftEyebrow*/, 80 /*rightEyebrow*/, 7 /*rightEyelid*/, 7 /*leftEyelid*/, 18 /*leftLip*/,
                 81 /*rightLip*/, 50 /*jaw*/, 67 /*neckTilt*/, -1 /*neckTwist*/);
    else
        SetState(50 /*leftHorizontalEye*/, 50 /*leftVerticalEye*/, 60 /* 50 rightHorizontalEye*/,
                 10 /* 50 rightVerticalEye*/, 50 /*leftEyebrow*/, 50 /*rightEyebrow*/, 50 /*rightEyelid*/,
                 50 /*leftEyelid*/, 50 /*leftLip*/, 50 /*rightLip*/, 0 /*jaw*/, 50 /*neckTilt*/, -1 /*neckTwist*/);
}

void Robot::SetExpression() {
    int value = (int) (rand() % 15);
    SetExpression(value);
}

//SetState(int n_leftHorizontalEye, int n_leftVerticalEye, int n_rightHorizontalEye, int n_rightVerticalEye, int n_leftEyebrow, int n_rightEyebrow, int n_rightEyelid, int n_leftEyelid, int n_leftLip, int n_rightLip, int n_jaw, int n_neckTilt, int n_neckTwist);
void Robot::SetLeft() {
    SetState(40, -1, 40, -1, 30, 70, 100, 100, 50, 50, 50, -1, 10);
}

void Robot::SetCentre() {
    SetState(40, -1, 40, -1, 50, 50, 100, 100, 50, 50, 50, -1, 50);
}

void Robot::SetRight() {
    SetState(40, -1, 40, -1, 30, 70, 100, 100, 50, 50, 50, -1, 90);
}

void Robot::SetNeck(int angle) {
    SetState(40, -1, 40, -1, 30, 70, 100, 100, 50, 50, 50, -1, angle);
}

//void Robot::SpeakMessage(std::string msg)
//{
//    SpeakWord(msg);
//    return;
//
//    QStringList words = msg.split(",",QString::SkipEmptyParts);
//    QStringListIterator iterator(words);
//    while (iterator.hasNext())
//    {
//        SpeakWord(iterator.next());
//        I::msleep(100);
//    }
//}

//void Robot::SpeakWord(std::string msg)
//{
//    Speak speak;
//    Robot robot(serial);
//
//    QStringList phons = speak.TextToPhon(msg);
//    speak.TextToSpeech(msg);
//
//    QStringListIterator iterator(phons);
//    while (iterator.hasNext())
//    {
//        QString shape = speak.GetMouthShape(iterator.next());
//        robot.SetMouth(shape);
//        I::msleep(60);
//    }
//}
