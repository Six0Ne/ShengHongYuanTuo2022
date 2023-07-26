#include "core/uart_test.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>

std::shared_ptr<Driver> driver = nullptr;
char BUFFtoTC246[10];
int servo = 100, motor = -5230;
int uart_send()
{
	//预留串口的设备名为/dev/ttyPS1
	driver = std::make_shared<Driver>("/dev/ttyPS1", BaudRate::BAUD_115200);
	if (driver == nullptr)
	{
		std::cout << "Create Driver Error ." << std::endl;
		return -1;
	}
	//串口初始化，打开串口设备及配置串口数据格式
	int ret = driver->open();
	if (ret != 0)
	{
		std::cout << "Driver Open failed ." << std::endl;
		return -1;
	}
	
//低位在前，高位在后
	BUFFtoTC246[0]=0x55;
	BUFFtoTC246[1]=0x51;
	BUFFtoTC246[2]=char(servo);
	BUFFtoTC246[3]=char(servo >> 8);
	BUFFtoTC246[4]=char(motor);
	BUFFtoTC246[5]=char(motor >> 8);
	BUFFtoTC246[6]=6;
	BUFFtoTC246[7]=156;
	BUFFtoTC246[8]=5;
	BUFFtoTC246[9]=13;

	while (1) 
	{
		//循环发送数据0-9，然后延时1s重新发送
		for (size_t i = 0; i < 10; i++)
		{
			driver->senddata(BUFFtoTC246[i]);
		}
	}

	return 0;

}

// #include <unistd.h>
// #include <fcntl.h>
// #include <termios.h>
// #include <iostream>
 
// #define DEV_NAME  "/dev/ttyPS1"
 
// int main (int argc, char *argv[])
// {
//     //此处有问题，发送命令后会出现误码，有时候会发送失败
// 	int fd;
//     int ifSuccess;
//     unsigned int servo = 800, motor = 1502;
//     char buf0, buf1, buf2, buf3, buf4, buf5;
//     char buf[10];
//     struct termios newtio;

//     buf0 = 0x55;
// 	buf1 = 0x52;
//     buf2 = char(motor);
//     buf3 = char(motor >> 8);
//     buf4 = char(servo);
//     buf5 = char(servo >> 8);

//     buf[0] = buf0;
//     buf[1] = buf1;
//     buf[2] = buf2;
//     buf[3] = buf3;
//     buf[4] = buf4;
//     buf[5] = buf5;

//     cfsetispeed(&newtio, B115200);
//     cfsetospeed(&newtio, B115200);
 
// 	fd = open(DEV_NAME, O_RDWR | O_NOCTTY);

//     tcflush(fd ,TCIFLUSH);
//     tcsetattr(fd, TCSANOW, &newtio);

// 	ifSuccess = write(fd, buf, 6);
//     if(ifSuccess < 0)
//     {
//         std::cout << "Failed" << std::endl;
//     }
 
//     close(fd);

// 	return(0);
// }