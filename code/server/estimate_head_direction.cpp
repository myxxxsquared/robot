
#include <opencv2/opencv.hpp>

#include <iostream>
#include <iomanip>

#include "LinearMath/Matrix3x3.h"

#include "head_pose_estimation.hpp"

using namespace std;
using namespace cv;

inline double todeg(double rad) {
    return rad * 180 / M_PI;
}


int main(int argc, char **argv)
{
    Mat frame;
    bool show_frame = false;
    bool use_camera = false;

    show_frame = true;
    use_camera = true;

    auto estimator = HeadPoseEstimation(argv[1]);

    VideoCapture video_in;

    if (use_camera) {
        video_in = VideoCapture(0);

        // adjust for your webcam!
        video_in.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        video_in.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
        estimator.focalLength = 500;
        estimator.opticalCenterX = 320;
        estimator.opticalCenterY = 240;

        if(!video_in.isOpened()) {
            cerr << "Couldn't open camera" << endl;
            return 1;
        }
    }

    while(true) {
        if(use_camera) {
            auto ok = video_in.read(frame);
            if (!ok) break;
        }


        auto all_features = estimator.update(frame);


        auto poses = estimator.poses();

        int i = 0;
        cout << "{";

        for(auto pose : poses) {



            pose = pose.inv();

            double raw_yaw, raw_pitch, raw_roll;
            tf::Matrix3x3 mrot(
                    pose(0,0), pose(0,1), pose(0,2),
                    pose(1,0), pose(1,1), pose(1,2),
                    pose(2,0), pose(2,1), pose(2,2));
            mrot.getRPY(raw_roll, raw_pitch, raw_yaw);

            raw_roll = raw_roll - M_PI/2;
            raw_yaw = raw_yaw + M_PI/2;

            double yaw, pitch, roll;

            roll = raw_pitch;
            yaw = raw_yaw;
            pitch = -raw_roll;

            cout << "\"face_" << i << "\":";
            cout << setprecision(1) << fixed << "{\"yaw\":" << todeg(yaw) << ", \"pitch\":" << todeg(pitch) << ", \"roll\":" << todeg(roll) << ",";
            cout << setprecision(4) << fixed << "\"x\":" << pose(0,3) << ", \"y\":" << pose(1,3) << ", \"z\":" << pose(2,3) << "},";

            i++;
        }
        cout << "}\n" << flush;

        if (show_frame) {
            imshow("headpose", estimator.drawDetections(frame, all_features, poses));
            if(use_camera) {
                waitKey(10);
            }
            else {
                while(waitKey(10) != 1048603) {}
                break;
            }
        }
    }

}


