//
// Created by Shikang on 2018/1/18.
//

#ifndef FRITZ_CONTROL_ROBOT_H
#define FRITZ_CONTROL_ROBOT_H

#include "Serial.h"

class CalibrationData {
public:
    int leftHorizontalEyeTrim;
    int leftHorizontalEyeMax;
    int leftHorizontalEyeMin;
    int leftHorizontalEyePin;
    bool leftHorizontalEyeEnabled;

    int leftVerticalEyeTrim;
    int leftVerticalEyeMax;
    int leftVerticalEyeMin;
    int leftVerticalEyePin;
    bool leftVerticalEyeEnabled;

    int rightHorizontalEyeTrim;
    int rightHorizontalEyeMax;
    int rightHorizontalEyeMin;
    int rightHorizontalEyePin;
    bool rightHorizontalEyeEnabled;

    int rightVerticalEyeTrim;
    int rightVerticalEyeMax;
    int rightVerticalEyeMin;
    int rightVerticalEyePin;
    bool rightVerticalEyeEnabled;

    int leftEyebrowTrim;
    int leftEyebrowMax;
    int leftEyebrowMin;
    int leftEyebrowPin;
    bool leftEyebrowEnabled;

    int rightEyebrowTrim;
    int rightEyebrowMax;
    int rightEyebrowMin;
    int rightEyebrowPin;
    bool rightEyebrowEnabled;

    int rightEyelidTrim;
    int rightEyelidMax;
    int rightEyelidMin;
    int rightEyelidPin;
    bool rightEyelidEnabled;

    int leftEyelidTrim;
    int leftEyelidMax;
    int leftexplicitEyelidMin;
    int leftEyelidPin;
    bool leftEyelidEnabled;

    int neckTiltTrim;
    int neckTiltMax;
    int neckTiltMin;
    int neckTiltPin;
    bool neckTiltEnabled;

    int neckTwistTrim;
    int neckTwistMax;
    int neckTwistMin;
    int neckTwistPin;
    bool neckexplicitTwistEnabled;

    int leftLipTrim;
    int leftLipMax;
    int leftLipMin;
    int leftLipPin;
    bool leftLipEnabled;

    int rightLipTrim;
    int rightLipMax;
    int rightLipMin;
    int rightLipPin;
    bool rightLipEnabled;

    int jawTrim;
    int jawMax;
    int jawMin;
    int jawPin;
    bool jawEnabled;

    int sonarTriggerPin;
    int sonarEchoPin;
    bool sonarEnabled;

    int irPin;
    bool irEnabled;

    explicit CalibrationData();
};

class RobotState {
public:
    float leftHorizontalEye;
    float leftVerticalEye;
    float rightHorizontalEye;
    float rightVerticalEye;
    float leftEyebrow;
    float rightEyebrow;
    float rightEyelid;
    float leftEyelid;
    float leftLip;
    float rightLip;
    float jaw;
    float neckTilt;
    float neckTwist;
    long position;
    long triggerPosition;

    RobotState() {
        leftHorizontalEye = 0.5f;
        leftVerticalEye = 0.5f;
        rightHorizontalEye = 0.5f;
        rightVerticalEye = 0.5f;
        leftEyebrow = 0.5f;
        rightEyebrow = 0.5f;
        rightEyelid = 0.5f;
        leftEyelid = 0.5f;
        leftLip = 0.5f;
        rightLip = 0.5f;
        jaw = 0.5f;
        neckTilt = 0.5f;
        neckTwist = 0.5f;
        position = 0;
        triggerPosition = 0;
    }

    explicit RobotState(bool empty) {
        leftHorizontalEye = -1;
        rightHorizontalEye = -1;
        leftVerticalEye = -1;
        rightVerticalEye = -1;
        leftEyebrow = -1;
        rightEyebrow = -1;
        leftEyelid = -1;
        rightEyelid = -1;
        leftLip = -1;
        rightLip = -1;
        jaw = -1;
        neckTilt = -1;
        neckTwist = -1;
        position = 0;
        triggerPosition = 0;
    }

    RobotState(const RobotState &r) {
        leftHorizontalEye = r.leftHorizontalEye;
        rightHorizontalEye = r.rightHorizontalEye;
        leftVerticalEye = r.leftVerticalEye;
        rightVerticalEye = r.rightVerticalEye;
        leftEyebrow = r.leftEyebrow;
        rightEyebrow = r.rightEyebrow;
        leftEyelid = r.leftEyelid;
        rightEyelid = r.rightEyelid;
        leftLip = r.leftLip;
        rightLip = r.rightLip;
        jaw = r.jaw;
        neckTilt = r.neckTilt;
        neckTwist = r.neckTwist;
        position = r.position;
        triggerPosition = r.triggerPosition;
    }

};

class Robot {
private:
    // send packets

    static const int minPin = 2;
    static const int maxPin = 19;

    static const int ARDUINO_GET_ID = 0; // returns ARDU
    static const int ARDUINO_RESET = 1; //
    static const int ARDUINO_SET_OBJECT = 2; //
    static const int ARDUINO_SET_SERVO = 3; //
    static const int ARDUINO_HEARTBEAT = 4;
    static const int ARDUINO_RELEASE_SERVO = 5;
    static const int ARDUINO_GET_IR = 6;
    static const int ARDUINO_GET_SONAR = 7;

    static const int ARDUINO_LOAD_CONFIG = 32;
    static const int ARDUINO_SAVE_CONFIG = 33;
    static const int ARDUINO_SAVE_SEQUENCE = 34;


    float f_leftHorizontalEye;
    float f_leftVerticalEye;
    float f_rightHorizontalEye;
    float f_rightVerticalEye;
    float f_leftEyebrow;
    float f_rightEyebrow;
    float f_rightEyelid;
    float f_leftEyelid;
    float f_leftLip;
    float f_rightLip;
    float f_jaw;
    float f_neckTilt;
    float f_neckTwist;

    int leftHorizontalEyeMin;
    int leftHorizontalEyeMax;
    int leftHorizontalEyePin;
    int leftVerticalEyeMin;
    int leftVerticalEyeMax;
    int leftVerticalEyePin;
    int rightHorizontalEyeMin;
    int rightHorizontalEyeMax;
    int rightHorizontalEyePin;
    int rightVerticalEyeMin;
    int rightVerticalEyeMax;
    int rightVerticalEyePin;
    int leftLipMin;
    int leftLipMax;
    int leftLipPin;
    int rightLipMin;
    int rightLipMax;
    int rightLipPin;
    int jawMin;
    int jawMax;
    int jawPin;
    int neckTiltMin;
    int neckTiltMax;
    int neckTiltPin;
    int neckTwistMin;
    int neckTwistMax;
    int neckTwistPin;
    int leftEyebrowMin;
    int leftEyebrowMax;
    int leftEyebrowPin;
    int rightEyebrowMin;
    int rightEyebrowMax;
    int rightEyebrowPin;
    int leftEyelidMin;
    int leftEyelidMax;
    int leftEyelidPin;
    int rightEyelidMin;
    int rightEyelidMax;
    int rightEyelidPin;

    int sonarOutPin;
    int sonarInPin;

    int irValue;
    double sonarValue;

    CalibrationData *cd;

    Serial *serial;


public:
    explicit Robot(Serial *serial);

    ~Robot();

    void Reset();

    double GetSonar();

    void SetState(int n_leftHorizontalEye, int n_leftVerticalEye, int n_rightHorizontalEye, int n_rightVerticalEye,
                  int n_leftEyebrow, int n_rightEyebrow, int n_rightEyelid, int n_leftEyelid, int n_leftLip,
                  int n_rightLip, int n_jaw, int n_neckTilt, int n_neckTwist);

    void SetExpression(std::string name);

    void SetExpression(int e);

    void SetExpression();

    void SetMouth(std::string shape);

    void SetLeft();

    void SetCentre();

    void SetRight();

    void SetNeck(int angle);

//    void SpeakMessage(std::string msg);
//    void SpeakWord(std::string word);
};


#endif //FRITZ_CONTROL_ROBOT_H
