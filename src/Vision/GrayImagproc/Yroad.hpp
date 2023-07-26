#pragma once
#ifndef YROAD_H_H
#define YROAD_H_H
#include <stdio.h>
enum yroad_type_e
{
	YROAD_NONE = 0,                     // 非三岔路
	YROAD_FOUND,                        // 找到三岔，即两个Y角点
	YROAD_NEAR,                         // 三岔距离较近，即两个Y角点较近
	GENERIC_IN,                         // 进三叉内部泛行区
	GENERIC_OUT,                         // 进三叉内部泛行区
	YROAD_NUM,
};

extern enum yroad_type_e Yroad_Type;

extern bool generic_Ypt0_found, generic_Ypt1_found;
extern int generic_Ypt0_rpts0s_id, generic_Ypt1_rpts1s_id;

//进入泛型区远线设定
extern int faryroad_ipts1[Generic_POINTS_MAX_LEN][2];//出泛型区找的远线
extern int faryroad_ipts1_num;

extern float faryroad_rpts1[Generic_POINTS_MAX_LEN][2];
extern int faryroad_rpts1_num;

extern float faryroad_rpts1b[Generic_POINTS_MAX_LEN][2];
extern int faryroad_rpts1b_num;

extern float faryroad_rpts1s[Generic_POINTS_MAX_LEN][2];
extern int faryroad_rpts1s_num;
//以下定义为泛行区寻线设定
extern int generic_ipts0[Generic_POINTS_MAX_LEN][2];//出泛型区找的远线
extern int generic_ipts1[Generic_POINTS_MAX_LEN][2];//泛行区内部线
extern int generic_ipts0_num, generic_ipts1_num;

extern float generic_rpts0[Generic_POINTS_MAX_LEN][2];
extern float generic_rpts1[Generic_POINTS_MAX_LEN][2];
extern int generic_rpts0_num, generic_rpts1_num;

extern float generic_rpts0b[Generic_POINTS_MAX_LEN][2];
extern float generic_rpts1b[Generic_POINTS_MAX_LEN][2];
extern int generic_rpts0b_num, generic_rpts1b_num;

extern float generic_rpts0s[Generic_POINTS_MAX_LEN][2];
extern float generic_rpts1s[Generic_POINTS_MAX_LEN][2];
extern int generic_rpts0s_num, generic_rpts1s_num;

extern float generic_rpts0a[Generic_POINTS_MAX_LEN];
extern float generic_rpts1a[Generic_POINTS_MAX_LEN];
extern int generic_rpts0a_num, generic_rpts1a_num;

extern float generic_rpts0an[Generic_POINTS_MAX_LEN];
extern float generic_rpts1an[Generic_POINTS_MAX_LEN];
extern int generic_rpts0an_num, generic_rpts1an_num;

void Check_Yroad();

void Run_Yroad();

void FarYroad_Line();//三叉扫远线，进入泛行区

void Generic_InLine(); //泛行区进线  

void Generic_OutLine();//泛行区出线 


#endif // YROAD_H