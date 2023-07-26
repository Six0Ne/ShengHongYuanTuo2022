/*********************************************************************************************************************
 * File name  : camera_param.hpp-摄像头矫正头文件
 * Target core: EB FZ3B
 *  Created on: 2021年4月25日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *            （16届上海交通大学巡线开源方案）
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#pragma once
#ifndef CAMERA_PARAM_H__H
#define CAMERA_PARAM_H__H
#include <stdbool.h>


extern float H[3][3];
extern float undistortmapx[200][320];
extern float undistortmapy[200][320];
extern float mapx[200][320];
extern float mapy[200][320];
extern int invx[200][320];
extern int invy[200][320];
bool map_inv(float pt0[2], int pt1[2]);

#endif // CAMERA_PARAM_H