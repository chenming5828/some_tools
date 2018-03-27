// This application receives image data from a drone and transpose data as a ros topic.

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>

#include "ros/ros.h"

#include "std_msgs/String.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include <thread>

#include <arpa/inet.h>

constexpr int udp_port = 20121;
int sock_fd;
struct sockaddr_in serv_addr;
image_transport::Publisher pub;
socklen_t serv_len;

bool gRunFlag = true;

struct param_image {
    int len;
    int w;
    int h;
};

inline void InitServConf()
{
    std::vector<std::string> vStrs = {"airbee", "shadeoff", "pointsoff", "labeloff", "kltoff", "caloff", "svooff", "jpg0000000000000098"};

    for_each(vStrs.begin(), vStrs.end(), [](std::string str){
        int send_num = sendto(sock_fd, str.c_str(), str.size(), 0, (struct sockaddr *)&serv_addr, serv_len);
        if (send_num != str.size()) {
            std::cerr << "Send message to server failed." << std::endl;
            return;
        }
        std::cout << "Sent airbee msg ..." << str << std::endl;
    });
}

void stream_image()
{
    sock_fd = socket(PF_INET, SOCK_DGRAM, 0);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.1");
    serv_addr.sin_port = htons(udp_port);

    serv_len = sizeof(serv_addr);

    bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    InitServConf();
    // char sendstr[7];
    // sprintf(sendstr, "airbee");
    // int send_num = sendto(sock_fd, sendstr, sizeof(sendstr), 0, (struct sockaddr *)&serv_addr, serv_len);
    // if (send_num != 7) {
    //     std::cerr << "Send message to server failed." << std::endl;
    //     return;
    // }
    // std::cout << "Sent airbee msg ..." << std::endl;

    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    //addr_serv.sin_port = htons(20175);
    client_addr.sin_port = htons(udp_port);
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t client_len = sizeof(struct sockaddr_in);

    while (gRunFlag)
    {
        InitServConf();

        char recv_buf[2048];

        int recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0,
            (struct sockaddr *)&client_addr, &client_len);

        // std::cout << "Recv something ..." << std::endl;

        if(recv_num < 0) {
            printf("--Airbee WARN image server recv error\n");
            continue;
        }

        if (recv_num == 18) {
            // Check header string
            if (recv_buf[0] == 'H' && recv_buf[1] == 'E' && recv_buf[2] == 'A' && recv_buf[3] == 'D' && recv_buf[4] == 'E' && recv_buf[5] == 'R') {
                // Get the paramters of image
                param_image pi;
                memcpy(&pi, recv_buf + 6, 12);
                int len = 0;
                uchar buff[pi.len];
                while (len < pi.len)
                {
                    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0,
                        (struct sockaddr *)&client_addr, &client_len);

                    if (recv_num < 0)
                        break;

                    memcpy(buff + len, recv_buf, recv_num);
                    len += recv_num;
                }

                if (len != pi.len)
                {
                    std::cout << "Can't get full image" << std::endl;
                    continue;
                }

                std::vector<uchar> vec_data(buff, buff + len);
                cv::Mat dst = cv::imdecode(vec_data, 0);

                sensor_msgs::ImagePtr msg;
                cv_bridge::CvImage t_left = cv_bridge::CvImage(std_msgs::Header(), "mono8", dst);

                // Fake time
                ros::Time msg_time;
                // msg_time.sec = (double)((int)timestamp);
                // msg_time.nsec = 1e9 * (timestamp - msg_time.sec);
                t_left.header.stamp = msg_time;
                t_left.header.seq = 0;

                msg = t_left.toImageMsg();

                pub.publish(msg);
            }
        }
    }
}

void interrupt(int sig)
{
    std::cout << "Get signal now" << std::endl;
    gRunFlag = false;
}

int main(int argc, char** argv)
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = interrupt;
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    ros::init(argc, argv, "my_udp_image");

	ros::NodeHandle n;

	image_transport::ImageTransport it(n);
	pub = it.advertise("/cam0/image_raw", 1);

    std::thread th(stream_image);

    ros::Rate loop_rate(50);

    while (ros::ok()) {
        ros::spinOnce();
        loop_rate.sleep();
    }

    if (th.joinable())
        th.join();

    return 0;
}

// EOF