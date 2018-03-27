#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;  
  
using namespace cv;  
  
int main( int argc, char** argv )
  
{  
  
     cv::VideoCapture capture("rotation.MP4");  
  
    if (!capture.isOpened())  
    {  
        std::cout << "Read video Failed !" << std::endl;  
        return -1;  
    }  
  
    cv::Mat frame;  
    cv::namedWindow("video test");  
  
    int frame_num = capture.get(cv::CAP_PROP_FRAME_COUNT);  
    std::cout << "total frame number is: " << frame_num << std::endl;  
  
    for (int i = 0; i < frame_num - 1; ++i)  
    {  
        capture >> frame;  
        //capture.read(frame); 第二种方式  
        imshow("video test", frame);  
        if (cv::waitKey(30) == 'q')  
        {  
            break;  
        }  
    }  
  
    cv::destroyWindow("video test");  
    capture.release();  
    return 0;  
  
}  