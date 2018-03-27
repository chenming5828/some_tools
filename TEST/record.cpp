#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
using namespace cv;
using namespace std;

Mat frame;
Mat gray;

int main()
{
     VideoCapture capture(0);
     if(!capture.isOpened ())
     {
         printf("open camera error\n");
         return 0;
     }

     capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);  
     capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240); 

    //  float gain = 0.2f;
    //  // float ev = 0.0002;
    //  capture.set(CV_CAP_PROP_FPS,30);
    //  capture.set(CV_CAP_PROP_AUTO_EXPOSURE,0.25);   //manual 0.25 auto 0.75
    //  capture.set(CV_CAP_PROP_EXPOSURE,0.04);      //后面值是最大值的比例
    //  capture.set(CV_CAP_PROP_GAIN,gain);            //后面值是最大值的比例
     // float exposure = capture.get(CV_CAP_PROP_EXPOSURE);
     // std::cout << exposure << std::endl;
     
     int cnt = 0;

     while(1)
     {
         capture >> frame;
 

         cnt ++;
         std::ostringstream ostr;

         ostr <<"pic-"<< std::setfill('0') << std::setw(7) << cnt << ".jpg";
         std::string name = ostr.str();
      


         imshow("camera",frame);

         imwrite("../image/"+name,frame);


         char c = (char)waitKey(1);
         if( c == 27 )
             break;
     }
     return 0;
}
