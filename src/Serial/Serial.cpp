#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <cstring>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/signal.h>
#include<fcntl.h>
#include "Serial.hpp"
#include "../Control/Pid.hpp"
using namespace std;
#define DEV_NAME  "/dev/ttyPS1"
// int fd;
int ifSuccess;
TC264Data TC264;
char buf[10];
struct termios newtio;
void Serial_Init()
{
	//端口初始化
	cfsetispeed(&newtio, B115200);
	cfsetospeed(&newtio, B115200);

	fd = open(DEV_NAME, O_RDWR | O_NOCTTY);
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);
}
bool buzzer = false;
int Serial_Send(int* servo, int* motor)
{
	// *servo=10;
	//cout << "servo = " << *servo << endl;
	buf[0] = 0x55;
	buf[1] = 0x51;
	buf[2] = (char)(*servo);
	buf[3] = (char)(*servo >> 8);
	buf[4] = (char)(*motor);
	buf[5] = (char)(*motor >> 8);
	buf[6] = 1;
	ifSuccess = write(fd, buf, 7);
	return ifSuccess;
}


void GetTc264Init(TC264Data* TC264RecData)
{
	TC264RecData->Speed = 0.0;
	TC264RecData->Ramp_Out = 0;
	TC264RecData->eulerAngle = 0;
	TC264RecData->i = 0;
	TC264RecData->TC264receiveComplete_Flag = 0;
}


void GetTc264Data(int fd, TC264Data* TC264RecData, PID* Servo)
{
	//
	memset(TC264RecData->rx, 0, sizeof(TC264RecData->rx));
	ifSuccess = read(fd, TC264RecData->rx, 10);                // 接收串口数据  存在在rx数组里
	tcflush(fd, TCIFLUSH); //一定要提高串口的响应速度
	if (TC264RecData->rx[0] != 0x55)//帧头检测 检测第1位
	{
		TC264RecData->i = 0;
		return;
	}
	else if (TC264RecData->rx[9] != 0x77) { return; }
	else
	{
		TC264RecData->eulerAngle = (short)((TC264RecData->rx[2] << 8) | TC264RecData->rx[1]);
		Servo->P = (short)((TC264RecData->rx[4] << 8) | TC264RecData->rx[3]) / 100.00;
		Servo->I = (short)((TC264RecData->rx[6] << 8) | TC264RecData->rx[5]) / 100.00;
		Servo->D = (short)((TC264RecData->rx[8] << 8) | TC264RecData->rx[7]) / 100.00;
		//tcflush(fd, TCIFLUSH);//读完数据清空缓存
		TC264RecData->TC264receiveComplete_Flag = 1;
	}
	//cout << "buf =  " << (short)TC264RecData->rx[1] << endl;

}
int serial_recv()
{
	char buf1[5];
	ifSuccess = read(fd, buf1, 5);
	cout << "buf =  " << (int)buf1[4] << endl;
	return ifSuccess;
}


//串口中断配置 5-14：没必要用
#define FALSE -1
#define TRUE 0
#define flag 1
#define noflag 0

int wait_flag = noflag;
int STOP = 0;
int res;

int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = { 38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 19200, 9600, 4800, 2400, 1200, 300, };
void set_speed(int fd, int speed)
{
	int i;
	int status;
	struct termios Opt;
	tcgetattr(fd, &Opt);
	for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
	{
		if (speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if (status != 0)
			{
				perror("tcsetattr fd1");
				return;
			}
			tcflush(fd, TCIOFLUSH);
		}
	}
}

int set_Parity(int fd, int databits, int stopbits, int parity)
{
	struct termios options;
	if (tcgetattr(fd, &options) != 0)
	{
		perror("SetupSerial 1");
		return (FALSE);
	}
	options.c_cflag &= ~CSIZE;
	switch (databits)
	{
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr, "Unsupported data size\n");
		return (FALSE);
	}
	switch (parity)
	{
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;	/* Clear parity enable */
		options.c_iflag &= ~INPCK;	/* Enable parity checking */
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);
		options.c_iflag |= INPCK;	/* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;	/* Enable parity */
		options.c_cflag &= ~PARODD;
		options.c_iflag |= INPCK;	/* Disnable parity checking */
		break;
	case 'S':
	case 's':			/*as no parity */
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf(stderr, "Unsupported parity\n");
		return (FALSE);
	}

	switch (stopbits)
	{
	case 1:
		options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		options.c_cflag |= CSTOPB;
		break;
	default:
		fprintf(stderr, "Unsupported stop bits\n");
		return (FALSE);
	}
	/* Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;
	tcflush(fd, TCIFLUSH);
	options.c_cc[VTIME] = 150;
	options.c_cc[VMIN] = 0;	/* Update the options and do it NOW */
	if (tcsetattr(fd, TCSANOW, &options) != 0)
	{
		perror("SetupSerial 3");
		return (FALSE);
	}
	return (TRUE);
}
char buff[255];
int fd = open(DEV_NAME, O_RDWR);
void signal_handler_IO(int status)
{
	//printf ("received SIGIO signale.\n");
	//memset (buff, 0, sizeof(buff));//开辟个内存空间储存串口发送的数组
	GetTc264Data(fd, &TC264, &PID_Servo);
	cout << "TC264.Direction_D =  " << PID_Servo.D << endl;
	//res = read (fd, buff, 255);
	//printf ("nread=%d,%s\n", res, buff);
	wait_flag = noflag;
}
void Serial_interruptionInit()
{
	//int fd;
	struct sigaction saio;
	//fd = open (DEV_NAME, O_RDWR);
	if (fd == -1)
	{
		perror("serialport error\n");
	}
	else
	{
		printf("open");
		printf("%s", ttyname(fd));
		printf(" succesfully\n");
	}

	saio.sa_handler = signal_handler_IO;
	sigemptyset(&saio.sa_mask);
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO, &saio, NULL);

	//allow the process to receive SIGIO
	fcntl(fd, F_SETOWN, getpid());
	//make the file descriptor asynchronous
	fcntl(fd, F_SETFL, FASYNC);

	set_speed(fd, 115200);
	if (set_Parity(fd, 8, 1, 'N') == FALSE)
	{
		printf("Set Parity Error\n");
		exit(0);
	}

}