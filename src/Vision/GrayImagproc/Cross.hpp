#pragma once
#ifndef CORSS_H_H
#define CORSS_H_H
#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
enum cross_type_e
{
	CROSS_NONE = 0,     // 非十字模式
	CROSS_BEGIN,        // 找到左右两个L角点
	CROSS_IN,           // 两个L角点很近，即进入十字内部(此时切换远线控制)
	CROSS_NUM,
};

extern enum cross_type_e Cross_Type;

extern bool far_Lpt0_found, far_Lpt1_found;
extern int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;

// 以下定义为十字寻远线设定
extern int far_ipts0[FAR_POINTS_MAX_LEN][2];
extern int far_ipts1[FAR_POINTS_MAX_LEN][2];
extern int far_ipts0_num, far_ipts1_num;

extern float far_rpts0[FAR_POINTS_MAX_LEN][2];
extern float far_rpts1[FAR_POINTS_MAX_LEN][2];
extern int far_rpts0_num, far_rpts1_num;

extern float far_rpts0b[FAR_POINTS_MAX_LEN][2];
extern float far_rpts1b[FAR_POINTS_MAX_LEN][2];
extern int far_rpts0b_num, far_rpts1b_num;

extern float far_rpts0s[FAR_POINTS_MAX_LEN][2];
extern float far_rpts1s[FAR_POINTS_MAX_LEN][2];
extern int far_rpts0s_num, far_rpts1s_num;

extern float far_rpts0a[FAR_POINTS_MAX_LEN];
extern float far_rpts1a[FAR_POINTS_MAX_LEN];
extern int far_rpts0a_num, far_rpts1a_num;

extern float far_rpts0an[FAR_POINTS_MAX_LEN];
extern float far_rpts1an[FAR_POINTS_MAX_LEN];
extern int far_rpts0an_num, far_rpts1an_num;

void Check_Cross();

void Run_Cross();

void Cross_FarLine();

void Cross_FarLine2();

#endif // CORSS_H