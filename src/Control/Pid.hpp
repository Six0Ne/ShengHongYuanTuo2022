#pragma once
#ifndef PID_H_H
#define PID_H_H
#include <iostream>
#include <string.h>
// #define MIN(a, b) (((a) < (b)) ? (a) : (b))
// #define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MINMAX(input, low, upper) MIN(MAX(input, low), upper)
typedef struct pid
{
	float P;
	float I;
	float D;
	float  Error;//误差
	float  Error_pre;//上次误差
	float  Error_Rate;//这次误差减上次误差
	float  Error_ADD;//误差的累加
	float  Limit;
	float  u;
} PID;
extern PID PID_Servo;
extern PID RGBPID_Servo;
extern PID Target_Speed_PID;
float Limit_Float(float amt, float low, float high);//限幅函数
void  PID_init(PID* PID_TYPE0,PID* PID_TYPE1,PID* PID_TYPE2);
float Servo_PID(PID* Servo, float Error);

#endif // PID_H