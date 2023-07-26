#pragma once
#ifndef SERIAL_H_H
#define SERIAL_H_H
#include <iostream>
#include <string.h>
#include "../Control/Pid.hpp"
typedef struct TC264 //接收数据结构体
{

	int Speed;
	bool Ramp_Out;
	float eulerAngle;
	unsigned char TC264receiveComplete_Flag;      //数据接收成功的标志位
	unsigned char rxtemp[1];
	unsigned char rx[10];                        //数据接收缓冲区
	unsigned char tx[10];
	unsigned char i;                           //当前接收数据位
} TC264Data;


extern int fd;
extern bool buzzer;
extern TC264Data TC264;
void Serial_Init();
int Serial_Send(int* servo, int* motor);
void GetTc264Init(TC264Data* TC264RecData);
void GetTc264Data(int fd, TC264Data* TC264RecData, PID* Servo);
int serial_recv();
void Serial_interruptionInit();

#endif // SERIAL_H_H