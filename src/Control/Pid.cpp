#include <opencv2/opencv.hpp>
#include <cmath>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "Pid.hpp"
PID PID_Servo;
PID RGBPID_Servo;
// 从赛道弯曲情况，计算目标速度
PID Target_Speed_PID;
//                            P         I     D      Limit
float PID_Unit[3][4] = { {    0.8,      0,    0,     60},//灰头方向环参数整定  基本没有静差 可以不加I
						 {  0.55,      0,    0.2,  60}, //彩头方向环参数整定
						 {  13.5,      0,    0.0,  80}, //速度规划
};
//-------------------------------------------------------------------------------------------------------------------
//  @brief        限幅函数
//  @author       CUCJUMITS_17_完全模型
//  data          4月2日
//  @return       int16
//  @since        v1.0
//  Sample usage: 比例P :    e(k)           误差
//                积分I :   e(I)+=e(k)     误差的累加
//                微分D :   e(k)-e(k-1)    这次误差-上次误差
//-------------------------------------------------------------------------------------------------------------------
float Limit_Float(float amt, float low, float high)//限幅函数
{
	return ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)));
}
//-------------------------------------------------------------------------------------------------------------------
//  brief      PID结构体变量初始化
//  author     CUCJUMITS_17_完全模型
//  date       4月20号
//  return     void
//  since      v1.0
//  Sample usage: 
//-------------------------------------------------------------------------------------------------------------------
void PID_init(PID* PID_TYPE0, PID* PID_TYPE1, PID* PID_TYPE2)//PID结构体变量初始化
{

	PID_TYPE0->P = PID_Unit[0][0];
	PID_TYPE0->I = PID_Unit[0][1];
	PID_TYPE0->D = PID_Unit[0][2];
	PID_TYPE0->Limit = PID_Unit[0][3];
	PID_TYPE0->Error = 0;
	PID_TYPE0->Error_ADD = 0;
	PID_TYPE0->Error_Rate = 0;
	PID_TYPE0->Error_pre = 0;

	PID_TYPE1->P = PID_Unit[1][0];
	PID_TYPE1->I = PID_Unit[1][1];
	PID_TYPE1->D = PID_Unit[1][2];
	PID_TYPE1->Limit = PID_Unit[1][3];
	PID_TYPE1->Error = 0;
	PID_TYPE1->Error_ADD = 0;
	PID_TYPE1->Error_Rate = 0;
	PID_TYPE1->Error_pre = 0;

	PID_TYPE2->P = PID_Unit[2][0];
	PID_TYPE2->I = PID_Unit[2][1];
	PID_TYPE2->D = PID_Unit[2][2];
	PID_TYPE2->Limit = PID_Unit[2][3];
	PID_TYPE2->Error = 0;
	PID_TYPE2->Error_ADD = 0;
	PID_TYPE2->Error_Rate = 0;
	PID_TYPE2->Error_pre = 0;

}
/*输入量P语言值特征点*/
float PFF[7] = { -30,-25,-10,0,10,25,30 };
/*输入量D语言值特征点*/
float DFF[5] = { -5,-2.5,0,2.5,5 };
/*输出量U语言值特征点(根据赛道类型选择不同的输出值)*/
float UFF[7] = { 0, 0.55, 0.58, 0.63, 0.65, 0.67, 0.69 };
/*采用了调整因子的规则表,大误差时偏重误差,小误差时偏重误差变化*/
int rule[7][5] = {
	//ec   -2  -1   0   1   2   e
		  { 6,  6,  5,  5,  4}, //  -3
		  { 5,  4,  3,  2,  2}, //  -2
		  { 4,  3,  2,  1,  1}, //  -1
		  { 4,  3,  0,  3,  4}, //   0
		  { 1,  1,  2,  3,  4}, //   1
		  { 2,  2,  3,  4,  5}, //   2
		  { 4,  5,  5,  6,  6}  //   3
};

float Fuzzy(float P, float D)    /*模糊运算引擎*/
{
	float U = 0;  /*偏差,偏差微分以及输出值的精确量*/
	float PF[2] = { 0 }, DF[2] = { 0 }, UF[4] = { 0 };
	/*偏差,偏差微分以及输出值的隶属度*/
	int Pn = 0, Dn = 0, Un[4] = { 0 };
	float t1 = 0, t2 = 0, t3 = 0, t4 = 0, temp1 = 0, temp2 = 0;
	/*隶属度的确定*/
	/*根据PD的指定语言值获得有效隶属度*/
	if (P > PFF[0] && P < PFF[6])
	{
		if (P <= PFF[1])
		{
			Pn = -2;
			PF[0] = (PFF[1] - P) / (PFF[1] - PFF[0]);
		}
		else if (P <= PFF[2])
		{
			Pn = -1;
			PF[0] = (PFF[2] - P) / (PFF[2] - PFF[1]);
		}
		else if (P <= PFF[3])
		{
			Pn = 0;
			PF[0] = (PFF[3] - P) / (PFF[3] - PFF[2]);
		}
		else if (P <= PFF[4])
		{
			Pn = 1;
			PF[0] = (PFF[4] - P) / (PFF[4] - PFF[3]);
		}
		else if (P <= PFF[5])
		{
			Pn = 2;
			PF[0] = (PFF[5] - P) / (PFF[5] - PFF[4]);
		}
		else if (P <= PFF[6])
		{
			Pn = 3;
			PF[0] = (PFF[6] - P) / (PFF[6] - PFF[5]);
		}
	}

	else if (P <= PFF[0])
	{
		Pn = -2;
		PF[0] = 1;
	}
	else if (P >= PFF[6])
	{
		Pn = 3;
		PF[0] = 0;
	}

	PF[1] = 1 - PF[0];


	//判断D的隶属度
	if (D > DFF[0] && D < DFF[4])
	{
		if (D <= DFF[1])
		{
			Dn = -2;
			DF[0] = (DFF[1] - D) / (DFF[1] - DFF[0]);
		}
		else if (D <= DFF[2])
		{
			Dn = -1;
			DF[0] = (DFF[2] - D) / (DFF[2] - DFF[1]);
		}
		else if (D <= DFF[3])
		{
			Dn = 0;
			DF[0] = (DFF[3] - D) / (DFF[3] - DFF[2]);
		}
		else if (D <= DFF[4])
		{
			Dn = 1;
			DF[0] = (DFF[4] - D) / (DFF[4] - DFF[3]);
		}
	}
	//不在给定的区间内
	else if (D <= DFF[0])
	{
		Dn = -2;
		DF[0] = 1;
	}
	else if (D >= DFF[4])
	{
		Dn = 1;
		DF[0] = 0;
	}

	DF[1] = 1 - DF[0];

	/*使用误差范围优化后的规则表rule[7][7]*/
	/*输出值使用13个隶属函数,中心值由UFF[7]指定*/
	/*一般都是四个规则有效*/
	Un[0] = rule[Pn - 1 + 3][Dn - 1 + 3];
	Un[1] = rule[Pn + 3][Dn - 1 + 3];
	Un[2] = rule[Pn - 1 + 3][Dn + 3];
	Un[3] = rule[Pn + 3][Dn + 3];

	if (PF[0] <= DF[0])    //求小   
		UF[0] = PF[0];
	else
		UF[0] = DF[0];
	if (PF[1] <= DF[0])
		UF[1] = PF[1];
	else
		UF[1] = DF[0];
	if (PF[0] <= DF[1])
		UF[2] = PF[0];
	else
		UF[2] = DF[1];
	if (PF[1] <= DF[1])
		UF[3] = PF[1];
	else
		UF[3] = DF[1];
	/*同隶属函数输出语言值求大*/
	if (Un[0] == Un[1])
	{
		if (UF[0] > UF[1])
			UF[1] = 0;
		else
			UF[0] = 0;
	}
	if (Un[0] == Un[2])
	{
		if (UF[0] > UF[2])
			UF[2] = 0;
		else
			UF[0] = 0;
	}
	if (Un[0] == Un[3])
	{
		if (UF[0] > UF[3])
			UF[3] = 0;
		else
			UF[0] = 0;
	}
	if (Un[1] == Un[2])
	{
		if (UF[1] > UF[2])
			UF[2] = 0;
		else
			UF[1] = 0;
	}
	if (Un[1] == Un[3])
	{
		if (UF[1] > UF[3])
			UF[3] = 0;
		else
			UF[1] = 0;
	}
	if (Un[2] == Un[3])
	{
		if (UF[2] > UF[3])
			UF[3] = 0;
		else
			UF[2] = 0;
	}
	t1 = UF[0] * UFF[Un[0]];
	t2 = UF[1] * UFF[Un[1]];
	t3 = UF[2] * UFF[Un[2]];
	t4 = UF[3] * UFF[Un[3]];
	temp1 = t1 + t2 + t3 + t4;
	temp2 = UF[0] + UF[1] + UF[2] + UF[3];//模糊量输出
	U = temp1 / temp2;
	return U;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief        位置式PID
//  @author       CUCJUMITS_17_完全模型
//  data          4月2日
//  @return       int16
//  @since        v1.0
//  Sample usage: 比例P :    e(k)           误差
//                积分I :   e(I)+=e(k)     误差的累加
//                微分D :   e(k)-e(k-1)    这次误差-上次误差
//-------------------------------------------------------------------------------------------------------------------
float Servo_PID(PID* Servo, float Error)
{
	Servo->Error = Error;//误差

	Servo->Error_ADD += Servo->Error;//误差的累加
	//Constrain_Float(Loca->I * Loca->Error_ADD, -Loca->Limit, Loca->Limit);//对积分项限幅  怕数据溢出

	Servo->Error_Rate = Servo->Error - Servo->Error_pre;//这次误差减上次误差


    Servo->u = (Servo->P * Servo->Error) + (Servo->D * Servo->Error_Rate);

   //Servo->P=Fuzzy(Servo->Error,Servo->Error_Rate);
	//Servo->u = (Servo->P * Servo->Error) + (Servo->I * Servo->Error_ADD) + (Servo->D * Servo->Error_Rate);

	Servo->Error_pre = Servo->Error;//上次误差

	Servo->u = Limit_Float(Servo->u, -Servo->Limit, Servo->Limit);

	return Servo->u;
}


