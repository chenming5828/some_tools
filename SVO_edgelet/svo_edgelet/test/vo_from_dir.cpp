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

#include <set>
#include <mutex>
#include <condition_variable>


bool g_pauseControl, g_goControl, g_resumeControl;

// mutex for one step debugging.
std::mutex g_Mtx;

std::condition_variable g_Cond;
using namespace std;


#define CALL_SWITCH_PAUSE()   \
    g_pauseControl = true;  \
    g_resumeControl = false;    \
    g_goControl = false;    

#define CALL_SWITCH_RESUME()  \
    g_resumeControl = true;                 \
    g_pauseControl = false;                   \
    g_goControl =false;

#define CALL_SWITCH_ONE_STEP()     \
    g_pauseControl = false;               \
    g_resumeControl = true;               \
    g_goControl = true;



//  enum Mode {
//     CONTINUS,
//     STEP,
//     PAUSE
//   };
// static Mode mode;

void PauseLcmDataSendSet()
{
    //Set parameter to switch to PAUSE modul.
    std::unique_lock<mutex> lck(g_Mtx);
    CALL_SWITCH_PAUSE();
}

// Resume
void ResumeLcmDataSendSet()
{
    //First need to make sure to step in the resume modul.
    std::unique_lock<mutex> lck(g_Mtx);
    CALL_SWITCH_RESUME();

    g_Cond.notify_all();
}

// One Step mode;
void GoOneStepSet()
{
    //Set parameter to swith to step by step modul.
    std::unique_lock<mutex> lck(g_Mtx);
    CALL_SWITCH_ONE_STEP();

    g_Cond.notify_all();
}

void ControlThread(void)
{
    char ch;

    while (true)
    {
        std::cin.get(ch);
        std::cout << ch << std::endl;
        if(ch == 's' || ch == 'S')
            PauseLcmDataSendSet();
        else if(ch == 'r' || ch == 'R')
            ResumeLcmDataSendSet();
        else if(ch == 'g' || ch == 'G')
            GoOneStepSet();
            // GoOneStepSet();
    }
}

class BenchmarkNode
{
    svo::AbstractCamera* cam_;
    svo::PinholeCamera* cam_pinhole_;
    svo::FrameHandlerMono* vo_;

    SLAM_VIEWER::Viewer* viewer_;
    std::thread * viewer_thread_;
    std::thread * ControlTh;

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
    ControlTh = new std::thread(ControlThread);

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
  static int  img_id = 0;
  for(;;)
  { 
        if(g_pauseControl) {
            unique_lock<mutex> lck(g_Mtx);
            while(!g_resumeControl && !g_goControl){
                g_Cond.wait(lck);
            }
        }

        if(g_goControl) {
            PauseLcmDataSendSet();
        }
    //   if(1)
      {
            img_id++;
            // load image
            std::stringstream ss;
            ss << "/home/sam/some_tools/TEST/image/pic-"<< std::setw(7) << std::setfill( '0' ) << img_id << ".jpg";
                
            if(img_id == 1)
                std::cout << "reading image " << ss.str() << std::endl;
            cv::Mat img(cv::imread(ss.str().c_str()));
            std::cout << img_id<< std::endl;
            assert(!img.empty());

            cv::imshow("origin_image", img);
            cv::waitKey(1);
            if(img_id < 100) continue;

            cv::cvtColor(img, img, CV_RGB2GRAY);

            cv::Mat image1;
            GaussianBlur(img, image1, cv::Size(3,3), 0);
            vo_->addImage(image1, 0.01*img_id);

        

            // display tracking quality
            if(vo_->lastFrame() != NULL)
            {
                // std::cout << "Frame-Id: " << vo_->lastFrame()->id_ << " \t"
                //           << "#Features: " << vo_->lastNumObservations() << " \t"
                //           << "Proc. Time: " << vo_->lastProcessingTime()*1000 << "ms \n";

                // access the pose of the camera via vo_->lastFrame()->T_f_w_.
            }
      }
    
     usleep(20000);
  }

 
   
    return;

}


int main(int argc, char** argv)
{


    BenchmarkNode benchmark;
    benchmark.runFromFolder();

    printf("BenchmarkNode finished.\n");
    return 0;
}