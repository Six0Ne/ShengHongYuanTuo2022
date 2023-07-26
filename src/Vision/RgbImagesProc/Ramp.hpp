/*********************************************************************************************************************
 * File name  : Rmap.hpp-坡道元素
 * Target core: EB FZ3B
 *  Created on: 2021年8月3日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *       
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#pragma once
#ifndef RAMP_H_H
#define RAMP_H_H

enum ramp_type_e
{
	RAMP_NONE = 0,     // 非坡道模式
	RAMP_FOUND,
	RAMP_IN,           // 两个L角点很近，即进入十字内部(此时切换远线控制)
	RAMP_OUT,
	RAMP_NUM,
};

extern enum ramp_type_e Ramp_Type;
extern Mat ramp_image_rgb;
// 压缩后灰原图左右边线
extern int ramp0[RAMP_POINTS_MAX_LEN][2];//左线
extern int ramp1[RAMP_POINTS_MAX_LEN][2];//右线
extern int ramp_mid[RAMP_POINTS_MAX_LEN][2];//中线
extern int ramp0_num, ramp1_num, ramp_mid_num;//左右边线像素点数
//图像爬边参数
extern float ramp_thres;//起始行阈值
extern float ramp_block_size;//根据分辨率大小调整
extern float ramp_clip_value;
extern float ramp_begin_x;//边线爬取的起始X，和跟踪起始点不太一样
extern float ramp_begin_y;//边线爬取的起始Y，和跟踪起始点不太一样


void Ramp_Proc(Mat& img0);
void Chrek_Ramp(void);
void Run_Ramp(void);
void Ramp_Disp(Mat& DISP);
#endif 
