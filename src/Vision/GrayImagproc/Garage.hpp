/*********************************************************************************************************************
 * File name  : garage.hpp-车库元素
 * Target core: EB FZ3B
 *  Created on: 2021年4月25日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *            （优化16届上海交通大学开源方案）
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#pragma once
#ifndef GARAGE_H_H
#define GARAGE_H_H
enum garage_type_e {
	GARAGE_NONE = 0,                        // 非车库模式
	GARAGE_BEGINOUT_LEFT, GARAGE_BEGINOUT_RIGHT,      // 准备出库，过渡阶段，避免压脚
	GARAGE_OUT_LEFT, GARAGE_OUT_RIGHT,      // 出库
	GARAGE_FOUND_LEFT, GARAGE_FOUND_RIGHT,  // 发现车库，即斑马线+单侧L角点(未使用)
	GARAGE_IN_LEFT, GARAGE_IN_RIGHT,        // 进库，发现车库后判断第几次，从而决定是否进库
	GARAGE_PASS_LEFT, GARAGE_PASS_RIGHT,    // 不进库，发现车库后判断第几次，从而决定是否进库
	GARAGE_STOP,                            // 进库完毕，停车
	GARAGE_NUM,
};

extern enum garage_type_e Garage_Type;

void Check_Garage();

void Run_Garage();

#endif