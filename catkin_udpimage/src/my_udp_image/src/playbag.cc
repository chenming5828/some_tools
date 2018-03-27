#include <thread>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <algorithm>
#include <set>
#include <mutex>
#include <condition_variable>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include "ros/ros.h"

#include "std_msgs/String.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

// Global Variables
bool g_pauseControl, g_goControl, g_resumeControl;

// mutex for one step debugging.
std::mutex g_Mtx;

std::condition_variable g_Cond;

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


using namespace std;

std::string g_imageFileFolder;
  
/* Show all files under dir_name , do not show directories ! */
std::vector<std::string> vecFileName;

image_transport::Publisher pub;

bool ExtractAllFiles(const char * dir_name)
{
    cout << dir_name << endl;

    // check the parameter !  
    if( NULL == dir_name )  
    {  
        cout<<" dir_name is null ! "<<endl;  
        return false;
    }  
  
    // check if dir_name is a valid dir  
    struct stat s;  
    lstat( dir_name , &s );  
    if(!S_ISDIR( s.st_mode ) )  
    {  
        cout<<"dir_name is not a valid directory !"<<endl;  
        return false;
    }
 
    struct dirent * filename;    // return value for readdir()  
    DIR * dir;                   // return value for opendir()  
    dir = opendir( dir_name );  
    if( NULL == dir )  
    {  
        cout << "Can not open dir " << dir_name << endl;  
        return false;  
    }

    cout << "Successfully opened the dir !" << endl;  
      
    /* read all the files in the dir ~ */  
    while( ( filename = readdir(dir) ) != NULL )  
    {
        // get rid of "." and ".."  
        if( strcmp( filename->d_name , "." ) == 0 ||   
            strcmp( filename->d_name , "..") == 0    )  
            continue;

        vecFileName.push_back(std::string(filename->d_name));
    }  
}

struct NumFile{
    int num;
    std::string file;
};

bool compareNumFile(const NumFile& d1_, const NumFile& d2_)
{
    return d1_.num < d2_.num;
}

std::set<NumFile, decltype(compareNumFile)*> gNumFileSet(compareNumFile);

bool ReadImageData()
{
    ExtractAllFiles(g_imageFileFolder.c_str());

    for_each(vecFileName.begin(), vecFileName.end(),[&](std::string& str){
        size_t p1 = str.find("pic");
        if (p1 != std::string::npos){
            size_t p2 = p1;
            for (; p1 < str.size(); p2++) {
                if (str[p2] == '-')
                    break;
            }
            std::string numstr = str.substr(p1 + 3, p2 - 3 - p1);
            NumFile f{atoi(numstr.c_str()), str};
            gNumFileSet.insert(f);
        }
    });

    return true;
}

// Pause
void PauseLcmDataSendSet()
{
    //Set parameter to switch to PAUSE modul.
    unique_lock<mutex> lck(g_Mtx);
    CALL_SWITCH_PAUSE();
}

// Resume
void ResumeLcmDataSendSet()
{
    //First need to make sure to step in the resume modul.
    unique_lock<mutex> lck(g_Mtx);
    CALL_SWITCH_RESUME();

    g_Cond.notify_all();
}

// One Step mode;
void GoOneStepSet()
{
    //Set parameter to swith to step by step modul.
    unique_lock<mutex> lck1(g_Mtx);
    CALL_SWITCH_ONE_STEP();

    g_Cond.notify_all();
}

void StreamImageData()
{
    for (auto& f : gNumFileSet) {
        if(g_pauseControl) {
            unique_lock<mutex> lck(g_Mtx);
            while(!g_resumeControl && !g_goControl){
                g_Cond.wait(lck);
            }
        }

        if(g_goControl) {
            PauseLcmDataSendSet();
        }

        std::string str1 = g_imageFileFolder + f.file;
        std::cout << "read image : " << str1 << std::endl;
        cv::Mat img = cv::imread(str1, 0);

        std::cout << "size : " << img.size() << std::endl;

        sensor_msgs::ImagePtr msg;
        cv_bridge::CvImage t_left = cv_bridge::CvImage(std_msgs::Header(), "mono8", img);

        // Fake time
        ros::Time msg_time;
        // msg_time.sec = (double)((int)timestamp);
        // msg_time.nsec = 1e9 * (timestamp - msg_time.sec);
        t_left.header.stamp = msg_time;
        t_left.header.seq = 0;

        msg = t_left.toImageMsg();

        pub.publish(msg);

        usleep(1e5);
    }

    std::cout << "Congratulations: Stream Loop Has Done!!!" << std::endl;
}

// Receiving command from keyboard and doing control
void ControlThread()
{
    // LOG(INFO) << "Command control thread started.";

    char ch;

    while (true)
    {
        cin.get(ch);

        if(ch == 's' || ch == 'S')
            PauseLcmDataSendSet();
        else if(ch == 'r' || ch == 'R')
            ResumeLcmDataSendSet();
        else if(ch == 'g' || ch == 'G')
            GoOneStepSet();
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage : a.out image_path" << std::endl;
        return -1;
    }

    if (std::string(argv[1]) == "--help") {
        std::cout << "Press key 's' to pause the program, 'r' to resume." << std::endl;
        std::cout << "Press 'g' while the program is paused, it will play a frame and wait for your next press." << std::endl;

        return 0;
    }

    g_imageFileFolder = std::string(argv[1]);

    ReadImageData();

    ros::init(argc, argv, "my_udp_image");

	ros::NodeHandle n;

	image_transport::ImageTransport it(n);
	pub = it.advertise("/cam0/image_raw", 1);

    std::thread StreamTh(StreamImageData);
    std::thread ControlTh(ControlThread);

    ros::Rate loop_rate(1);

    while (ros::ok()) {
        ros::spinOnce();
        loop_rate.sleep();
    }

    StreamTh.join();
    ControlTh.join();

    return 1;
}

// EOF