// This file is part of SVO - Semi-direct Visual Odometry.
//
// Copyright (C) 2014 Christian Forster <forster at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// SVO is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or any later version.
//
// SVO is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <svo/config.h>
#include <svo/frame_handler_mono.h>
#include <svo/map.h>
#include <svo/frame.h>
#include <vector>
#include <string>
#include <svo/math_lib.h>
#include <svo/camera_model.h>
#include <opencv2/opencv.hpp>
#include <sophus/se3.h>
#include <iostream>

#include <svo/slamviewer.h>
#include<thread>
#include <sys/time.h>

class BenchmarkNode
{
    svo::AbstractCamera* cam_;
    svo::PinholeCamera* cam_pinhole_;
    svo::FrameHandlerMono* vo_;

    SLAM_VIEWER::Viewer* viewer_;
    std::thread * viewer_thread_;

public:
    BenchmarkNode();
    ~BenchmarkNode();
    void runFromFolder();
};

BenchmarkNode::BenchmarkNode()
{
   
    // cam_ = new svo::PinholeCamera(640,480,530.219436,530.039372, 310.848872, 239.993128, -0.105126, 0.202058, 0.000813, -0.001571, 0.000000);
    //cam_ = new svo::PinholeCamera(320,240,530.219436 / 2.0,530.039372 / 2.0, 310.848872 / 2.0, 239.993128 / 2.0, -0.105126, 0.202058, 0.000813, -0.001571, 0.000000);
    // cam_ = new svo::PinholeCamera(640,480,279.411,280.368408, 311.887933, 204.439799, -0.263329, 0.039389, 0.000776, 0.004244, 0.000000);
     cam_ = new svo::PinholeCamera(320,240,146.785622,148.255304, 162.839915, 109.118409, 0.042797, 0.0684, -0.000683,  0.003840,0);// 180    

    vo_ = new svo::FrameHandlerMono(cam_);
    vo_->start();

    viewer_ = new SLAM_VIEWER::Viewer(vo_);
    viewer_thread_ = new std::thread(&SLAM_VIEWER::Viewer::run,viewer_);
    viewer_thread_->detach();

}

BenchmarkNode::~BenchmarkNode()
{
    delete vo_;
    delete cam_;
    delete cam_pinhole_;

    delete viewer_;
    delete viewer_thread_;
}

//#define TXTREAD
void BenchmarkNode::runFromFolder()
{

    cv::VideoCapture cap(0);  // open the default camera

    if (!cap.isOpened())  // check if we succeeded
        return ;

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

    int img_id = 0;
    for (;;) {

        cv::Mat image;
        cap.read(image);  // get a new frame from camera

        assert(!image.empty());
        img_id++;
        
        cv::imshow("origin_image", image);
        // if (cv::waitKey(1) >= 0) break;
        cv::waitKey(1);
        if(img_id < 100) continue;

        cv::cvtColor(image, image, CV_BGR2GRAY);
        // std::cout << "image size : " << image.size() << std::endl;
        /*
        cv::Mat unimg;
        cam_pinhole_->undistortImage(image,unimg);
        vo_->addImage(unimg, 0.01*img_id);
         */
        struct  timeval    tv0,tv1;
        struct  timezone   tz;
         
        gettimeofday(&tv0,&tz);
        cv::Mat image1;
        GaussianBlur(image, image1, cv::Size(3,3), 0);
        vo_->addImage(image1, 0.01*img_id);
        gettimeofday(&tv1,&tz);
        // printf("-------------------------track time : %f ms----------------\n",(tv1.tv_sec * 1e3 + tv1.tv_usec / 1e3) - (tv0.tv_sec * 1e3 + tv0.tv_usec/1e3));

        // display tracking quality
        if(vo_->lastFrame() != NULL)
        {
            // std::cout << "Frame-Id: " << vo_->lastFrame()->id_ << " \t"
            //           << "#Features: " << vo_->lastNumObservations() << " \n";
            // //<< "Proc. Time: " << vo_->lastProcessingTime()*1000 << "ms \n";
            // std::cout<<"Frame pose: "<< vo_->lastFrame()->T_f_w_ <<std::endl;

            // Vector3d ori(0,0,0);
            // Vector3d pos = vo_->lastFrame()->T_f_w_.inverse() * ori;
            
        }

    }

    cap.release();
    return;

}


int main(int argc, char** argv)
{


    BenchmarkNode benchmark;
    benchmark.runFromFolder();

    printf("BenchmarkNode finished.\n");
    return 0;
}

