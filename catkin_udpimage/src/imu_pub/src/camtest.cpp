#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "loitorusb.h"
#include "loitorcam.h"
#include "loitorimu.h"

#include "ros/ros.h"
#include "sensor_msgs/Imu.h"

using namespace std;
using namespace cv;
bool close_img_viewer=false;
bool visensor_Close_IMU_viewer=false;

// 当前左右图像的时间戳
timeval left_stamp,right_stamp;

/**
 * @brief opencv_showimg
 * @return
 */
void *opencv_showimg(void*)
{
    Mat img_left(Size(visensor_img_width(),visensor_img_height()),CV_8UC1);
    double left_timestamp;
    Mat img_right(Size(visensor_img_width(),visensor_img_height()),CV_8UC1);
    double right_timestamp;
    visensor_imudata img_imudata;
    while(!close_img_viewer)
    {
        if(visensor_is_leftcam_open())
        {
            if(visensor_is_left_img_new())
            {
                visensor_get_left_latest_img(img_left.data,&left_timestamp,&img_imudata);
                printf("L-Time: %8.6f, IMUTime: %8.6f\n",left_timestamp,img_imudata.timestamp);
                // imshow("left",img_left);
            }
        }
        if(visensor_is_rightcam_open())
        {
            if(visensor_is_right_img_new())
            {
                visensor_get_right_latest_img(img_right.data,&right_timestamp);
                printf("R-Time: %8.6f\n",right_timestamp);
                // imshow("right",img_right);
            }
        }
        waitKey(1);
    }
    pthread_exit(NULL);
}


void* show_imuData(void *)
{
    visensor_imudata imudata;


    while(!visensor_Close_IMU_viewer)
    {
        // printf("---------\n");
        if(visensor_imu_have_fresh_data())
        {
            visensor_get_imudata_latest(&imudata);
            printf("IMUTime:%8.6f, Gyr: %8.4f,%8.4f,%8.4f, Acc: %8.4f,%8.4f,%8.4f, Quat(WXYZ): %8.4f,%8.4f,%8.4f,%8.4f\n",
                   imudata.timestamp,
                   imudata.rx,imudata.ry,imudata.rz,
                   imudata.ax,imudata.ay,imudata.az,
                   imudata.qw,imudata.qx,imudata.qy,imudata.qz);
        }
        // else{
        //     printf("---------\n");
        // }
        usleep(100);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{

    visensor_load_settings("/home/sam/imu_pub/Loitor_VISensor_Setups.txt");

    ros::init(argc, argv, "imu");
    ros::NodeHandle n;
    ros::Publisher imu_data_pub_ = n.advertise<sensor_msgs::Imu>("imu_data", 100);
    ros::Rate loop_rate(50);



    int r = visensor_Start_Cameras();
    if(r<0)
    {
        printf("Opening cameras failed...\r\n");
        return r;
    }
    /************************** Start IMU **************************/
    int fd=visensor_Start_IMU();
    if(fd<0)
    {
        printf("visensor_open_port error...\r\n");
        return 0;
    }
    printf("visensor_open_port success...\r\n");
    /************************ ************ ************************/

   

    while (ros::ok())
    {
        visensor_imudata imudata;
        if(visensor_imu_have_fresh_data())
        {
            visensor_get_imudata_latest(&imudata);
            sensor_msgs::Imu msg;
            msg.header.frame_id = "imu";
            msg.header.stamp = ros::Time::now();

   
            msg.linear_acceleration.x = imudata.ax;
            msg.linear_acceleration.y = imudata.ay;
            msg.linear_acceleration.z = imudata.az;

         
            msg.angular_velocity.x = imudata.rx;
            msg.angular_velocity.y = imudata.ry;
            msg.angular_velocity.z = imudata.rz;

            msg.orientation.x = 0;
            msg.orientation.y = 0;
            msg.orientation.z = 0;
            msg.orientation.w = 1;
            imu_data_pub_.publish(msg);
            printf("IMUTime:%8.6f, Gyr: %8.4f,%8.4f,%8.4f, Acc: %8.4f,%8.4f,%8.4f, Quat(WXYZ): %8.4f,%8.4f,%8.4f,%8.4f\n",
                imudata.timestamp,
                imudata.rx,imudata.ry,imudata.rz,
                imudata.ax,imudata.ay,imudata.az,
                imudata.qw,imudata.qx,imudata.qy,imudata.qz);
        }
    
        
        ros::spinOnce();
        loop_rate.sleep();
    }



    /* close cameras */
    visensor_Close_Cameras();
    /* close IMU */
    visensor_Close_IMU();

    return 0;
}
