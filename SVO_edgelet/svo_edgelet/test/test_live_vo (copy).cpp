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

#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()

#define BUF_LEN 65540 // Larger than maximum UDP packet size

#include "opencv2/opencv.hpp"
using namespace cv;

#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80

unsigned short servPort = 8888;
UDPSocket sock(servPort);

char buffer[BUF_LEN]; // Buffer for echo string
int recvMsgSize; // Size of received message
string sourceAddress; // Address of datagram source
unsigned short sourcePort; // Port of datagram source

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
    /*
    cam_pinhole_ = new svo::PinholeCamera(640,480,502.961104,503.651566, 284.978460, 247.527333,
                                          -0.378740,0.133422, -0.001505, -0.001445);
    cam_ = new svo::PinholeCamera(640,480,407.763641, 453.693298, 267.111836,247.958895);
     */

    //  cam_model: Pinhole
    //  cam_width: 640
    //  cam_height: 480
    //  cam_fx: 268.248193 
    //  cam_fy: 269.249066 
    //  cam_cx: 327.402140 
    //  cam_cy: 193.063310 
    //  cam_d0: -0.258023  
    //  cam_d1: 0.040974 
    //  cam_d2: 0.004172 
    //  cam_d3: 0.002983

    // cam_model: Pinhole
    // cam_width: 640
    // cam_height: 480
    // cam_fx: 530.219436
    // cam_fy: 530.039372
    // cam_cx: 310.848872
    // cam_cy: 239.993128
    // cam_d0: -0.105126 
    // cam_d1: 0.202058
    // cam_d2: 0.000813
    // cam_d3: -0.001571


    // cam_model: Pinhole
    // cam_width: 640
    // cam_height: 480
    // cam_fx: 296.464686
    // cam_fy: 297.205879
    // cam_cx: 318.709280
    // cam_cy: 254.871764
    // cam_d0: -0.238517   
    // cam_d1: 0.039171
    // cam_d2: -0.001265
    // cam_d3: -0.000027

    
    // cam_ = new svo::PinholeCamera(640,480,268.248193,269.249066, 327.402140, 193.063310, -0.258023,0.040974,0.004172,-0.002983,0.0);
    cam_ = new svo::PinholeCamera(640,480,296.464686,297.205879, 318.709280, 254.871764, -0.238517,0.039171,-0.001265,-0.000027,0.0);
    
    // cam_ = new svo::PinholeCamera(640,480,530.219436,530.039372, 310.848872, 239.993128, -0.105126,0.202058,0.000813,-0.001571,0.0);
    

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

    // cv::VideoCapture cap(1);  // open the default camera

    // if (!cap.isOpened())  // check if we succeeded
    //     return ;

    int img_id = 0;
    for (;;) {

        // cv::Mat image;
        // cap.read(image);  // get a new frame from camera

        do {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
          } while (recvMsgSize > sizeof(int));
          int total_pack = ((int * ) buffer)[0];
      
          // cout << "expecting length of packs:" << total_pack << endl;
          char * longbuf = new char[PACK_SIZE * total_pack];
          for (int i = 0; i < total_pack; i++) {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            if (recvMsgSize != PACK_SIZE) {
                cerr << "Received unexpected size pack:" << recvMsgSize << endl;
                continue;
            }
            memcpy( & longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
        }
      
        // cout << "Received packet from " << sourceAddress << ":" << sourcePort << endl;
      
        Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
        Mat image = imdecode(rawData, CV_LOAD_IMAGE_GRAYSCALE);
        if (image.size().width == 0 ) {
            cerr << "decode failure!" << endl;
            free(longbuf);
            continue;
        }
        if (image.empty() ) {
            cerr << "ccccc" << endl;
            free(longbuf);
            continue;
        }

        // assert(!image.empty());
        img_id++;

        cv::imshow("origin_image", image);
        if (cv::waitKey(1) >= 0) break;
        if(img_id < 100)
        {
           
            free(longbuf); 
            continue;
        } 
        

        // cv::cvtColor(image,image,CV_BGR2GRAY);
        /*
        cv::Mat unimg;
        cam_pinhole_->undistortImage(image,unimg);
        vo_->addImage(unimg, 0.01*img_id);
         */
        vo_->addImage(image, 0.01*img_id);
        free(longbuf); 

        // display tracking quality
        if(vo_->lastFrame() != NULL)
        {
            std::cout << "Frame-Id: " << vo_->lastFrame()->id_ << " \t"
                      << "#Features: " << vo_->lastNumObservations() << " \n";
            //<< "Proc. Time: " << vo_->lastProcessingTime()*1000 << "ms \n";
            std::cout<<"Frame pose: "<< vo_->lastFrame()->T_f_w_ <<std::endl;

        }

    }

    // cap.release();
    return;

}


int main(int argc, char** argv)
{


    BenchmarkNode benchmark;
    benchmark.runFromFolder();

    printf("BenchmarkNode finished.\n");
    return 0;
}

