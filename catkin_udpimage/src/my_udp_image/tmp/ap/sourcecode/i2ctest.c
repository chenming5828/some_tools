#include <stdio.h>  
#include <linux/i2c.h>  
#include <linux/i2c-dev.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/ioctl.h>  
#include <string.h>
#define WRITE_MODE 0
#define WRITE_LENGTH 1
#define I2C0_FILENAME "/dev/i2c-0"
#define I2C1_FILENAME "/dev/i2c-1"
#define I2C2_FILENAME "/dev/i2c-2"
#define I2C3_FILENAME "/dev/i2c-3"
#define I2C4_FILENAME "/dev/i2c-4"
#define I2C5_FILENAME "/dev/i2c-5"
#define I2C6_FILENAME "/dev/i2c-6"
#define I2C7_FILENAME "/dev/i2c-7"
#define I2C8_FILENAME "/dev/i2c-8"
#define I2C9_FILENAME "/dev/i2c-9"
#define I2C10_FILENAME "/dev/i2c-10"
#define I2C11_FILENAME "/dev/i2c-11"
#define I2C12_FILENAME "/dev/i2c-12"
void i2c_write(int file, unsigned char addr, unsigned char *buf);
int main(int argc, char** argv){
	
	int i2c_file;
	char buf[10];
	int port;
	int addr;
	char val;
	int len;
	/*open a i2c device*/
	port = atoi(argv[1]);
	addr = atoi(argv[2]);
	val = atoi(argv[3]);
	printf("argv[1]:%dargv[2]:%d argv[3]:%d\n",port,addr,val);
	if((i2c_file = open(I2C11_FILENAME,O_RDWR)) < 0){
		printf("open i2c file fail\n");
		exit(1);
	}
	/*write i2c device*/
    i2c_write(i2c_file,addr,&val);
}
void i2c_write(int file,unsigned char addr,unsigned char *buf){
	struct i2c_rdwr_ioctl_data pak;
	struct i2c_msg msg;
	int res;
	msg.addr  = addr;
	msg.flags = WRITE_MODE;
	msg.len   = WRITE_LENGTH;
	msg.buf   = buf;
	pak.msgs  = &msg;
	pak.nmsgs = 1;
	res = ioctl(file,I2C_RDWR,&pak);
	if(res < 0){
		printf("send i2c data fail res:%d\n",res);
	}
}
