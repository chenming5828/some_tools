#include <opencv2/opencv.hpp>    
using namespace cv;    
using namespace std; 

Mat frame;

int main()
{
    VideoCapture capture(0);
    if(!capture.isOpened ())
    {
        printf("open camera error\n");
        return 0;
    }
    float gain = 0.f;
    capture.set(CV_CAP_PROP_FPS,30);
    capture.set(CV_CAP_PROP_AUTO_EXPOSURE,0.25);   //manual 0.25  auto 0.75
    capture.set(CV_CAP_PROP_EXPOSURE,0.1);      //后面值是最大值的比例
    capture.set(CV_CAP_PROP_GAIN,0);            //后面值是最大值的比例
    int exposure = capture.get(CV_CAP_PROP_EXPOSURE); 
    printf("exposure:: %d \n",exposure);
    int cnt = 0;
    while(1)    
    {   
        capture >> frame; 
        Mat gray;

        cvtColor(frame, gray, COLOR_RGB2GRAY);
        cnt ++;
        // imshow("camera",frame);
        if(cnt >20)
        {
            float sum = 0.f;
            for(int i=0;i<gray.rows;i++)
            {
                uchar *p = gray.ptr<uchar>(i);
                for(int j=0;j<gray.cols;j++)
                {
                    int tmp = p[j];
                    sum += tmp;
                }
            }
            std::cout << "gain :    "<< gain << "   "<< "brightness :   " << sum << std::endl;
            gain +=0.1f;
            capture.set(CV_CAP_PROP_GAIN,gain); 

            cnt = 0;
        }
        char c = (char)waitKey(10);  
        if( c == 27 )  
            break;  
    }
    return 0;
}