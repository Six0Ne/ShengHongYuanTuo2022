/*********************************************************************************************************************
 * File name  : circle.cpp-环岛元素
 * Target core: EB FZ3B
 *  Created on: 2021年4月25日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *            （优化16届上海交通大学开源方案）
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
enum circle_type_e Circle_Type = CIRCLE_NONE;

int none_left_line = 0, none_right_line = 0;
int have_left_line = 0, have_right_line = 0;


void Check_Circle()
{
	// none_left_line = 0, none_right_line = 0;
	// have_left_line = 0, have_right_line = 0;
	// 非圆环模式下，单边L角点，单边长直道
	if (Circle_Type == CIRCLE_NONE && Lpt0_found && !Lpt1_found && is_straight1)
	{
		Circle_Type = CIRCLE_LEFT_BEGIN;
		none_left_line = 0, none_right_line = 0;
		have_left_line = 0, have_right_line = 0;
	}
	if (Circle_Type == CIRCLE_NONE && !Lpt0_found && Lpt1_found && is_straight0)
	{
		Circle_Type = CIRCLE_RIGHT_BEGIN;
		none_left_line = 0, none_right_line = 0;
		have_left_line = 0, have_right_line = 0;
	}
}

void Run_Circle()
{

	// 左环开始，寻外直道右线
	//aim_distance = 0.40;//修改预瞄点距离
	buzzer = true;//执行元素，蜂鸣器叫
	if (Circle_Type == CIRCLE_LEFT_BEGIN)
	{
		Track_Type = TRACK_RIGHT;//一开始寻右线

		//先丢左线后有线
		if (rpts0s_num < 0.3 / sample_dist) { none_left_line++; }//发现圆环特征后，采样后的左边线小于20cm，丢线计算
		if (rpts0s_num > 0.5 / sample_dist && none_left_line > 3)//之前左边线丢线超过三帧图片，现在左边线长度大于1m,可以调试这个值
		{
			have_left_line++;
			if (have_left_line > 3) //切换寻巡线模式，标志位清0
			{
				Circle_Type = CIRCLE_LEFT_IN;
				none_left_line = 0;
				have_left_line = 0;
			}
		}
	}
	//入环，寻内圆左线
	else if (Circle_Type == CIRCLE_LEFT_IN)
	{
		Track_Type = TRACK_LEFT;
		if (rpts1s_num < 0.2 / sample_dist)
		{
			none_right_line++;
		}
		if (rpts1s_num > 0.5 / sample_dist && none_right_line > 3)//
		{
			have_right_line++;
			if (have_right_line > 3) //切换寻巡线模式，标志位清0
			{
				Circle_Type = CIRCLE_LEFT_RUNNING;
				none_right_line = 0;
				have_right_line = 0;
			}
		}
	}
	//正常巡线，寻外圆右线
	else if (Circle_Type == CIRCLE_LEFT_RUNNING)
	{
		// aim_distance = 0.45;//修改预瞄点距离
		Track_Type = TRACK_LEFT;

		if (Lpt1_found) rpts1s_num = rptsc1_num = Lpt1_rpts1s_id;//中线和边线截断到角点处
		//外环拐点（右L点）
		if (Lpt1_found && Lpt1_rpts1s_id < 0.5 / sample_dist) //发现右拐点，寻内圈
		{
			Circle_Type = CIRCLE_LEFT_OUT;//准备出环
		}
	}
	//出环，寻内环
	else if (Circle_Type == CIRCLE_LEFT_OUT)
	{
		Track_Type = TRACK_LEFT;

		//右线为长直道
		if (is_straight1 && !Lpt1_found)
		{
			Circle_Type = CIRCLE_LEFT_END;
		}
	}
	//走过圆环寻右线
	else if (Circle_Type == CIRCLE_LEFT_END)
	{
		Track_Type = TRACK_RIGHT;

		//左线先丢后有
		if (rpts0s_num < 0.2 / sample_dist) { none_left_line++; }
		if (rpts0s_num > 0.7 / sample_dist && none_left_line > 3)
		{
			Circle_Type = CIRCLE_NONE;
			none_left_line = 0, none_right_line = 0;
			have_left_line = 0, have_right_line = 0;
		}
	}
	//右环控制，前期寻左直道
	else if (Circle_Type == CIRCLE_RIGHT_BEGIN)
	{
		Track_Type = TRACK_LEFT;

		//先丢右线后有线
		if (rpts1s_num < 0.2 / sample_dist) { none_right_line++; }
		if (rpts1s_num > 0.5 / sample_dist && none_right_line > 3)
		{
			have_right_line++;
			if (have_right_line > 3)
			{
				Circle_Type = CIRCLE_RIGHT_IN;
				none_right_line = 0;
				have_right_line = 0;
			}
		}
	}
	//入右环，寻右内圆环
	else if (Circle_Type == CIRCLE_RIGHT_IN)
	{
		Track_Type = TRACK_RIGHT;

		if (rpts0s_num < 0.2 / sample_dist) { none_left_line++; }
		if (rpts0s_num > 0.5 / sample_dist && none_left_line > 3)
		{
			have_left_line++;
			if (have_left_line > 3)
			{
				Circle_Type = CIRCLE_RIGHT_RUNNING;
				none_left_line = 0;
				have_left_line = 0;
			}
		}

	}
	//正常巡线，寻外圆左线
	else if (Circle_Type == CIRCLE_RIGHT_RUNNING)
	{
		//aim_distance = 0.45;//修改预瞄点距离
		Track_Type = TRACK_LEFT;
		//外环存在拐点,可再加拐点距离判据(左L点)
		if (Lpt0_found) rpts0s_num = rptsc0_num = Lpt0_rpts0s_id;
		if (Lpt0_found && Lpt0_rpts0s_id < 0.5 / sample_dist)
		{
			Circle_Type = CIRCLE_RIGHT_OUT;
		}
	}
	//出环，寻内圆
	else if (Circle_Type == CIRCLE_RIGHT_OUT)
	{
		Track_Type = TRACK_RIGHT;
		if (is_straight0&&!Lpt0_found)
		{
			Circle_Type = CIRCLE_RIGHT_END;
		}
	}
	//走过圆环，寻左线
	else if (Circle_Type == CIRCLE_RIGHT_END)
	{
		Track_Type = TRACK_LEFT;
		//右线先丢后又
		if (rpts1s_num < 0.2 / sample_dist) { none_right_line++; }
		if (rpts1s_num > 0.7 / sample_dist && none_right_line > 3)
		{
			Circle_Type = CIRCLE_NONE;
			none_left_line = 0, none_right_line = 0;
			have_left_line = 0, have_right_line = 0;
		}
	}
}



