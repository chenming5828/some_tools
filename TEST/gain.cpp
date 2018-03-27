#include <opencv2/opencv.hpp>    
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
    float gain = 0.2f;
    float ev = 0.0002;
    capture.set(CV_CAP_PROP_FPS,30);
    capture.set(CV_CAP_PROP_AUTO_EXPOSURE,0.25);   //manual 0.25  auto 0.75
    // capture.set(CV_CAP_PROP_EXPOSURE,0.01);      //后面值是最大值的比例
    // capture.set(CV_CAP_PROP_GAIN,gain);            //后面值是最大值的比例

    // float exposure = capture.get(CV_CAP_PROP_EXPOSURE); 
    // std::cout << exposure << std::endl;
    // int cnt = 0;
    // for(int i = 0;i <256; i++)
    // {   
    //     float low = (i & 0xF) / 16.0f + 1;
    //     // std::cout << low<< std::endl;

    //     int high =((i & 0x10)/16 +1) * ((i & 0x20)/32 +1) * ((i & 0x40) /64 +1) * ((i & 0x80)/128+1);
    //     // std::cout << high<< std::endl;
        
    //     float result = high * low;

    //     std::cout << result << std::endl;

    // }

    // float f;
    // for(int j=0;j<256;j++)
    // {
    //     f = (j+0.5f)/256;

    //     f = (float)pow(f,1.9);
    //     int tmp = (uint8_t)(f*256-0.5f);
    //     std::cout << tmp << ",";
    // }


    while(1)    
    {   
        capture >> frame; 
        // cvtColor(frame, gray, COLOR_RGB2GRAY);

        // cnt ++;
        imshow("camera",frame);
        // if(cnt >20)
        // {
            
        //     cvtColor(frame, gray, COLOR_RGB2GRAY);
        //     float sum = 0.f;
        //     for(int i=0;i<gray.rows;i++)
        //     {
        //         uchar *p = gray.ptr<uchar>(i);
        //         for(int j=0;j<gray.cols;j++)
        //         {
        //             int tmp = p[j];
        //             sum += tmp;
        //         }
        //     }
        //     // std::cout << "gain :    "<< gain << "   "<< "brightness :   " << sum << std::endl;

        //     std::cout << sum <<"\t"<<capture.get(CV_CAP_PROP_EXPOSURE)<< std::endl;
        //     //   printf("exposure:: %f \n",exposure);
        //     // std::cout<< exposure << std::endl;
        //     ev +=0.0002;
        //     capture.set(CV_CAP_PROP_EXPOSURE,ev); 

        //     cnt = 0;
        // }

        char c = (char)waitKey(1);  
        if( c == 27 )  
            break;  
    }
    return 0;
}