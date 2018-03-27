#include <opencv2/opencv.hpp>    
using namespace cv;    
using namespace std; 


int main()
{
     Mat image = imread( "/home/sam/TEST/2.jpg", IMREAD_UNCHANGED);
    if(image.empty())
    {
        std::cerr << "Cannot read image file: "  << std::endl;
        return -1;
    }
    imshow("camera",image);

    float sum = 0.f;
    for(int i=0;i<image.rows;i++)
    {
        uchar *p = image.ptr<uchar>(i);
        for(int j=0;j<image.cols;j++)
        {
            int tmp = p[j];
            sum += tmp;
        }
    }
    std::cout << sum/image.rows/image.cols << std::endl;

     waitKey(0);  
}


