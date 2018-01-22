
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include "LinearMath/Matrix3x3.h"
#include "head_pose_estimation.hpp"

#include "headdirection.hpp"
#include "debug_throw.hpp"

using namespace std;
using namespace cv;

inline double todeg(double rad)
{
    return rad * 180 / M_PI;
}

#include <thread>
#include <mutex>

const char *const VIDEO_ADDRESS = "tcp://192.168.1.101:9000";
const char *const HEAD_POSE_DATA = "../data/shape_predictor_68_face_landmarks.dat";

HeadDirection::HeadDirection(SocketPipe *p)
    : pipe(p), started(false)
{
}

void HeadDirection::capture_proc_static(HeadDirection *obj)
{
    obj->capture_proc();
}

void HeadDirection::process_proc_static(HeadDirection *obj)
{
    obj->process_proc();
}

void HeadDirection::start()
{
    std::thread capture{HeadDirection::capture_proc_static, this};
    std::thread process{HeadDirection::process_proc_static, this};

    this->thread_capture = std::move(capture);
    this->thread_process = std::move(process);
}

void HeadDirection::capture_proc()
{
    VideoCapture cap{VIDEO_ADDRESS};
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened())
        my_throw("VideoCapture failed.");

    Mat *m1, *m2, *temp;
    m1 = new Mat();
    m2 = new Mat();

    while (true)
    {
        temp = m1;
        m1 = m2;
        m2 = temp;

        while (!cap.read(*m1))
            ;

        {
            std::unique_lock<std::mutex> lock{mutex_frame};
            curframe = m1;
            started = true;
        }
    }
}

void HeadDirection::process_proc()
{
    Mat frame;
    auto estimator = HeadPoseEstimation(HEAD_POSE_DATA);
    estimator.focalLength = 500;
    estimator.opticalCenterX = 320;
    estimator.opticalCenterY = 240;

    while(true)
    {
        {
            while(!started);
            std::unique_lock<std::mutex> lock{mutex_frame};
            frame = *curframe;
        }

        auto all_features = estimator.update(frame);
        auto poses = estimator.poses();

        for (auto pose : poses)
        {
            pose = pose.inv();
            double raw_yaw, raw_pitch, raw_roll;
            tf::Matrix3x3 mrot(
                pose(0, 0), pose(0, 1), pose(0, 2),
                pose(1, 0), pose(1, 1), pose(1, 2),
                pose(2, 0), pose(2, 1), pose(2, 2));
            mrot.getRPY(raw_roll, raw_pitch, raw_yaw);
            raw_roll = raw_roll - M_PI / 2;
            raw_yaw = raw_yaw + M_PI / 2;

            double yaw, pitch, roll;
            roll = raw_pitch;
            yaw = raw_yaw;
            pitch = -raw_roll;

            // throw "TODO: SEND MESSAGE HERE";
            // TODO: SEND MESSAGE HERE
            // cout << setprecision(1) << fixed << "{\"yaw\":" << todeg(yaw) << ", \"pitch\":" << todeg(pitch) << ", \"roll\":" << todeg(roll) << ",";
            // cout << setprecision(4) << fixed << "\"x\":" << pose(0, 3) << ", \"y\":" << pose(1, 3) << ", \"z\":" << pose(2, 3) << "},";
        }
    }
}
