#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> //文件控制定义
#include <termios.h>//终端控制定义
#include <errno.h>

#define BAUDRATE        B115200
#define FALSE  -1 
#define TRUE   0

int serial_fd = 0;
const char uartbuf[]="/dev/tty";

int test_uart_port(char* cDevicesName);

int main(int argc, char **argv){
	int fd;
	int data_tx = 0x58FF;
	char cDeviceName[20] = {0};
	int iUartNum = atoi(argv[2]);

	switch (*argv[1]) {
		case 'g':
		case 'G':

			break; 
 
		case 'i':
		case 'I': 

			break;  

		case 'u':
		case 'U':
			if (12 == iUartNum) {
				sprintf(cDeviceName, "%sHSL0", uartbuf);
			} else if (5 == iUartNum) {
				sprintf(cDeviceName, "%sHSL1", uartbuf);
			} else if (9 == iUartNum) {
				sprintf(cDeviceName, "%sHSL2", uartbuf);
			} else if (11 == iUartNum) {
				sprintf(cDeviceName, "%sHS0", uartbuf);
			} else {
				sprintf(cDeviceName, "%s%s", uartbuf, argv[2]);
			}

			printf("UART device name %s \n", cDeviceName); 
			test_uart_port(cDeviceName);

			break;

		default:   
			printf("Input incorrect Port Type\n");    
			exit (0);	
	}
	
	return 0;
}

/**************************************************************
/* 打开串口并初始化设置							
/* cDevicesName:设备名称
**************************************************************/
int init_serial(char* cDevicesName)
{
	serial_fd = open(cDevicesName, O_RDWR | O_NOCTTY | O_NDELAY);
	if (serial_fd < 0) {
	   perror("open");
	   return -1;
	}
 
	//串口主要设置结构体termios <termios.h>
	struct termios options;
 
	/**1. tcgetattr函数用于获取与终端相关的参数。
	*参数fd为终端的文件描述符，返回的结果保存在termios结构体中
	*/
	tcgetattr(serial_fd, &options);
	/**2. 修改所获得的参数*/
	options.c_cflag |= (CLOCAL | CREAD);//设置控制模式状态，本地连接，接收使能
	options.c_cflag &= ~CSIZE;//字符长度，设置数据位之前一定要屏掉这个位
	options.c_cflag &= ~CRTSCTS;//无硬件流控
	options.c_cflag |= CS8;//8位数据长度
	options.c_cflag &= ~CSTOPB;//1位停止位
	options.c_iflag |= IGNPAR;//无奇偶检验位
	options.c_oflag = 0; //输出模式
	options.c_lflag = 0; //不激活终端模式
	cfsetospeed(&options, B115200);//设置波特率
 
	/**3. 设置新属性，TCSANOW：所有改变立即生效*/
	tcflush(serial_fd, TCIFLUSH);//溢出数据可以接收，但不读
	tcsetattr(serial_fd, TCSANOW, &options);
 
	return 0;
}
 
/**************************************************************
/* 串口发送数据							
/* fd:串口描述符
/* data:待发送数据
/* datalen:数据长度
**************************************************************/
int uart_send(int fd, char *data, int datalen)
{
	int len = 0;
	len = write(fd, data, datalen);//实际写入的长度
	if(len == datalen) {
		printf("uart send OK!\n");
		return len;
	} else {
		tcflush(fd, TCOFLUSH);//TCOFLUSH刷新写入的数据但不传送
		return -1;
	}
 
	return 0;
}
 
/**************************************************************
/* 串口接收数据							
/* 要求启动后，在pc端发送ascii文件
/* fd:串口描述符
/* data:接收数据存储地址
/* datalen:接收数据长度
**************************************************************/
int uart_recv(int fd, char *data, int datalen)
{
	int len=0, ret = 0;
	fd_set fs_read;
	struct timeval tv_timeout;
 
	FD_ZERO(&fs_read);
	FD_SET(fd, &fs_read);
	tv_timeout.tv_sec  = (10*20/115200+2);
	tv_timeout.tv_usec = 0;
 
	ret = select(fd+1, &fs_read, NULL, NULL, &tv_timeout);
	//如果返回0，代表在描述符状态改变前已超过timeout时间,错误返回-1

	if (FD_ISSET(fd, &fs_read)) {
		len = read(fd, data, datalen);
		if (len > 0) {
			printf("uart receive OK!\n");
		}
		return len;
	} else {
		return -1;
	}
 
	return 0;
}

/**************************************************************
/* uartc测试函数						
/* cDevicesName:设备名称
**************************************************************/
int test_uart_port(char* cDevicesName)
{
	int iRet = 0;
	char buf[]="uart-test";
	char buf1[10];
 	int iTimeNum = 0;

	iRet = init_serial(cDevicesName);
	if (-1 == iRet) {
		printf("init serial failed\n");
		return -1;
	}

	while (iTimeNum < 10) {
		uart_send(serial_fd, buf, sizeof(buf));
		uart_recv(serial_fd, buf1, sizeof(buf));
		iTimeNum++;	
		sleep(0.2);
	}

	close(serial_fd);
	return 0;
}
