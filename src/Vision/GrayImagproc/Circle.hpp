/*********************************************************************************************************************
 * File name  : circle.hpp-环岛元素
 * Target core: EB FZ3B
 *  Created on: 2021年4月25日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *            （16届上海交通大学开源方案）
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#pragma once
#ifndef CIRCLE_H__H
#define CIRCLE_H__H
#include <stdio.h>
enum circle_type_e
{
	CIRCLE_NONE = 0,                           
	CIRCLE_LEFT_BEGIN, CIRCLE_RIGHT_BEGIN,      
	CIRCLE_LEFT_IN, CIRCLE_RIGHT_IN,            
	CIRCLE_LEFT_RUNNING, CIRCLE_RIGHT_RUNNING,  
	CIRCLE_LEFT_OUT, CIRCLE_RIGHT_OUT,          
	CIRCLE_LEFT_END, CIRCLE_RIGHT_END,          
	CIRCLE_NUM,                                 
};


extern enum circle_type_e Circle_Type;

void Check_Circle();

void Run_Circle();

#endif // CIRCLE_H