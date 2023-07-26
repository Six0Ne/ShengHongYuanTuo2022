/*********************************************************************************************************************
 * File name  : garage.cpp-车库元素
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
enum garage_type_e Garage_Type = GARAGE_BEGINOUT_LEFT;

// 记录当前第几次车库
int garage_num = 0;

float angle_diff(float a1, float a2)
{
	float c1 = cosf(a1 / 180 * PI);
	float s1 = sinf(a1 / 180 * PI);
	float c2 = cosf(a2 / 180 * PI);
	float s2 = sinf(a2 / 180 * PI);
	float c = c1 * c2 + s1 * s2;
	float s = s1 * c2 - s2 * c1;
	return atan2f(s, c) * 180 / PI;
}

int zebra_cross_flag_begin = 0;
int zebra_cross_flag0[30];
int zebra_cross_flag0_num = 0;
int zebra_cross_flag1[30];
int zebra_cross_flag1_num = 0;

float(*garage_rpts)[2];
int garage_rpts_num;

void Check_Garage()
{
	// 非车库模式下识别车库
	if (Garage_Type == GARAGE_NONE)
	{
		// 根据当前的L角点情况决定，取左(右)中线，进行斑马线的寻找
		if (Lpt0_found && !Lpt1_found)
		{
			// 仅左角点，则在右中线上找斑马线
			garage_rpts = rptsc1;
			garage_rpts_num = rptsc1_num;
		}
		else if (Lpt1_found && !Lpt0_found)
		{
			// 仅右角点，则在左中线上找斑马线
			garage_rpts = rptsc0;
			garage_rpts_num = rptsc0_num;
		}
		else
		{
			// 其余情况不找
			garage_rpts_num = 0;
		}
		int pt[2];
		// 没有能找的中线就退出
		if (garage_rpts_num == 0) return;
		// 在中线上一定范围内找斑马线
		for (int i = 15; i < MIN(100, garage_rpts_num); i++)
		{
			// 反变换后，超出图像范围则跳过
			if (!map_inv(garage_rpts[i], pt)) return;//把俯视域的中线变换到原视域（矫正后）
			// 调试绘图
			//draw_x(&img_raw, pt[0], pt[1], 3, 0);

			zebra_cross_flag_begin = image_src.at<uchar>(pt[1], pt[0]) > thres;
			//cout << "zebra_cross_flag_begin = " << zebra_cross_flag_begin << endl;
			memset(zebra_cross_flag0, 0, sizeof(zebra_cross_flag0));//把数组压入栈
			zebra_cross_flag0_num = 0;
			for (int x = pt[0] - 1; x >= MAX(0, pt[0] - 50); x--) //这个减50可以参考begin_x，注意不能判断到车体和地标就行,需要考虑一下会不会跟地标起冲突
			{
				if (zebra_cross_flag_begin == 0)
				{ // even white, odd black
					if (zebra_cross_flag0_num % 2 == 0 && image_src.at<uchar>(pt[1], x) > thres)
					{ // current even, current white
						zebra_cross_flag0[zebra_cross_flag0_num]++;
					}
					else if (zebra_cross_flag0_num % 2 == 0 && image_src.at<uchar>(pt[1], x) < thres)
					{ // current even, current black
						zebra_cross_flag0[++zebra_cross_flag0_num]++;
					}
					else if (zebra_cross_flag0_num % 2 == 1 && image_src.at<uchar>(pt[1], x) > thres)
					{ // current odd, current white
						zebra_cross_flag0[++zebra_cross_flag0_num]++;
					}
					else if (zebra_cross_flag0_num % 2 == 1 && image_src.at<uchar>(pt[1], x) < thres)
					{ // current odd, current black
						zebra_cross_flag0[zebra_cross_flag0_num]++;
					}
				}
				else
				{ // even black, odd white
					if (zebra_cross_flag0_num % 2 == 0 && image_src.at<uchar>(pt[1], x) > thres)
					{ // current even, current white
						zebra_cross_flag0[++zebra_cross_flag0_num]++;
					}
					else if (zebra_cross_flag0_num % 2 == 0 && image_src.at<uchar>(pt[1], x) < thres)
					{ // current even, current black
						zebra_cross_flag0[zebra_cross_flag0_num]++;
					}
					else if (zebra_cross_flag0_num % 2 == 1 && image_src.at<uchar>(pt[1], x) > thres)
					{ // current odd, current white
						zebra_cross_flag0[zebra_cross_flag0_num]++;
					}
					else if (zebra_cross_flag0_num % 2 == 1 && image_src.at<uchar>(pt[1], x) < thres)
					{ // current odd, current black
						zebra_cross_flag0[++zebra_cross_flag0_num]++;
					}
				}

			}

			memset(zebra_cross_flag1, 0, sizeof(zebra_cross_flag1));
			zebra_cross_flag1_num = 0;
			for (int x = pt[0] + 1; x <= MIN(image_src.cols - 1, pt[0] + 50); x++) //避免扫到地标
			{
				if (zebra_cross_flag_begin == 0) { // even white, odd black
					if (zebra_cross_flag1_num % 2 == 0 && image_src.at<uchar>(pt[1], x) > thres)
					{ // current even, current white
						zebra_cross_flag1[zebra_cross_flag1_num]++;
					}
					else if (zebra_cross_flag1_num % 2 == 0 && image_src.at<uchar>(pt[1], x) < thres)
					{ // current even, current black
						zebra_cross_flag1[++zebra_cross_flag1_num]++;
					}
					else if (zebra_cross_flag1_num % 2 == 1 && image_src.at<uchar>(pt[1], x) > thres)
					{ // current odd, current white
						zebra_cross_flag1[++zebra_cross_flag1_num]++;
					}
					else if (zebra_cross_flag1_num % 2 == 1 && image_src.at<uchar>(pt[1], x) < thres)
					{ // current odd, current black
						zebra_cross_flag1[zebra_cross_flag1_num]++;
					}
				}
				else { // even black, odd white
					if (zebra_cross_flag1_num % 2 == 0 && image_src.at<uchar>(pt[1], x) > thres)
					{ // current even, current white
						zebra_cross_flag1[++zebra_cross_flag1_num]++;
					}
					else if (zebra_cross_flag1_num % 2 == 0 && image_src.at<uchar>(pt[1], x) < thres)
					{ // current even, current black
						zebra_cross_flag1[zebra_cross_flag1_num]++;
					}
					else if (zebra_cross_flag1_num % 2 == 1 && image_src.at<uchar>(pt[1], x) > thres)
					{ // current odd, current white
						zebra_cross_flag1[zebra_cross_flag1_num]++;
					}
					else if (zebra_cross_flag1_num % 2 == 1 && image_src.at<uchar>(pt[1], x) < thres)
					{ // current odd, current black
						zebra_cross_flag1[++zebra_cross_flag1_num]++;
					}
				}
			}

			// 判断连续跳变的阈值条件以识别斑马线
			int i0 = 2;
			for (; i0 < zebra_cross_flag0_num - 1; i0++)
			{
				//斑马线太密调小这个阈值
				if (zebra_cross_flag0[i0] < 3 || zebra_cross_flag0[i0] >= 20 || abs(zebra_cross_flag0[i0 + 1] - zebra_cross_flag0[i0]) >= 10) break;
				// cout << "i0 = " << i0 << endl;
				// cout << "zebra_cross_flag0 = " << zebra_cross_flag0[i0] << endl;
			}

			// cout << "i0 = " << i0 << endl;
			//斑马线太少，调小这个阈值
			bool is_zebra0 = i0 > 6;//连续跳变阈值大于6次


			int i1 = 2;
			for (; i1 < zebra_cross_flag1_num - 1; i1++)
			{
				if (zebra_cross_flag1[i1] < 3 || zebra_cross_flag1[i1] >= 20 || abs(zebra_cross_flag1[i1 + 1] - zebra_cross_flag1[i1]) >= 10) break;
				// cout << "i1 = " << i0 << endl;
				// cout << "zebra_cross_flag1 = " << zebra_cross_flag1[i1] << endl;
			}
			// cout << "i1 = " << i1 << endl;
			bool is_zebra1 = i1 > 6;

			if (is_zebra0 && is_zebra1)
			{
				if (Lpt0_found && !Lpt1_found)
				{
					if (++garage_num >= 2)
					{    // 第二次车库就入库
						Garage_Type = GARAGE_IN_LEFT;
						//garage_yaw = eulerAngle.yaw;
						PID_Servo.P *= 2.5;
					}
					else
					{
						aim_distance = 0.75;                  // 第一次车库就不入库
						Garage_Type = GARAGE_PASS_LEFT;
						//garage_encoder = get_total_encoder();
					}
					break;
				}
				else if (!Lpt0_found && Lpt1_found)
				{
					if (++garage_num >= 2)
					{    // 第二次车库就入库
						Garage_Type = GARAGE_IN_RIGHT;
						//garage_yaw = eulerAngle.yaw;
						PID_Servo.P *= 2.5;
					}
					else
					{
						aim_distance = 0.75;             // 第一次车库就不入库
						Garage_Type = GARAGE_PASS_RIGHT;
						//garage_encoder = get_total_encoder();
					}
					break;
				}
			}
		}
	}
}
int NO_left_line = 0, NO_right_line = 0;
int GOT_left_line = 0, GOT_right_line = 0;

void Run_Garage()
{
	switch (Garage_Type)
	{
	case GARAGE_BEGINOUT_LEFT:
		Track_Type = TRACK_LEFT;
		//cout << "Lpt0_rpts0s_id = " <<Lpt0_rpts0s_id << endl;
		// if (Lpt0_found) 
		// {
		rpts0s_num = Lpt0_rpts0s_id;//把线截断到L角点处
		rptsc0_num = MAX(3, rpts0s_num - 5);
		rpts1s_num = Lpt1_rpts1s_id;
		rptsc1_num = MAX(3, rpts1s_num - 5);//把线截断到L角点处
		// }
		// if (Lpt1_found) 
		// {
		//     rpts1s_num = Lpt1_rpts1s_id;
		//     rptsc1_num = MAX(2,rpts1s_num-1);//把线截断到L角点处
		// }

		// //aim_distance = 0.65;//修改预瞄点距离

		if ((Lpt0_rpts0s_id != 0 && Lpt0_rpts0s_id < 0.1 / sample_dist))
		{
			Garage_Type = GARAGE_OUT_LEFT;//切换为进入十字模式
			//not_have_line = 0;
			//cross_encoder = current_encoder;//没用上
		}
		break;
	case GARAGE_OUT_LEFT:
		//aim_distance = 0.40;
		Track_Type = TRACK_LEFT;
		if (rpts1s_num < 0.15 / sample_dist) { NO_right_line++; }//出库训左线，采样后的左边线小于20cm，丢线计算
		if (rpts1s_num > 0.50 / sample_dist && NO_right_line > 2)//先丢线后有线
		{
			GOT_right_line++;
		}
		// 左边线无角点，右边线先丢线后有线出库成功
		if (!Lpt0_found && GOT_right_line > 1)
		{
			Garage_Type = GARAGE_NONE;
		}
		break;
	case GARAGE_OUT_RIGHT:
		aim_distance = 0.35;
		Track_Type = TRACK_RIGHT;
		// 右边线无角点，出库成功（实际调试更改条件）
		if (rpts0s_num < 0.35 / sample_dist) { NO_left_line++; }//出库训左线，采样后的左边线小于20cm，丢线计算
		if (rpts0s_num > 1.0 / sample_dist && NO_left_line > 2)//先丢线后有线
		{
			GOT_left_line++;
		}
		if (!Lpt1_found && GOT_left_line > 1)
		{
			Garage_Type = GARAGE_NONE;
		}
		break;
	case GARAGE_IN_LEFT://入库中寻左线，右边线先丢线后有线
		aim_distance = 0.40;
		Track_Type = TRACK_LEFT;
		if (rpts1s_num < 0.10 / sample_dist) { NO_right_line++; }//出库训左线，采样后的左边线小于20cm，丢线计算
		if (rpts1s_num > 0.50 / sample_dist && NO_right_line > 1)//先丢线后有线
		{
			GOT_right_line++;//
		}
		// 右边线先丢线后有线且识别到双L角点，则入库完毕
		if (GOT_right_line > 3 && Lpt0_found && Lpt0_rpts0s_id < 20 && Lpt1_found && Lpt1_rpts1s_id < 20)
		{
			Garage_Type = GARAGE_STOP;
			//rt_mb_send(buzzer_mailbox, 2);
		}
		break;
	case GARAGE_IN_RIGHT:
		aim_distance = 0.50;
		Track_Type = TRACK_RIGHT;
		if (rpts0s_num < 0.35 / sample_dist) { NO_left_line++; }//出库训左线，采样后的左边线小于20cm，丢线计算
		if (rpts0s_num > 1.0 / sample_dist && NO_left_line > 2)//先丢线后有线
		{
			GOT_left_line++;//
		}
		// 左边线先丢线后有线且识别到双L角点，则入库完毕
		if (GOT_left_line > 1 && Lpt0_found && Lpt0_rpts0s_id < 20 && Lpt1_found && Lpt1_rpts1s_id < 20)
		{
			Garage_Type = GARAGE_STOP;
			//rt_mb_send(buzzer_mailbox, 2);
		}
		break;
	case GARAGE_PASS_LEFT:
		Track_Type = TRACK_RIGHT;
		// 不入库，通过编码器使得小车走过车库后才退出车库模式
		if (rpts0s_num < 0.2 / sample_dist) { NO_left_line++; }//出库训左线，采样后的左边线小于20cm，丢线计算
		if (rpts0s_num > 1.0 / sample_dist && NO_left_line > 2)//先丢线后有线
		{
			GOT_left_line++;//
		}
		//左边线先丢线后有线
		if (GOT_left_line > 10)
		{
			Garage_Type = GARAGE_NONE;
		}
		break;
	case GARAGE_PASS_RIGHT:
		Track_Type = TRACK_LEFT;
		if (rpts1s_num < 0.4 / sample_dist) { NO_right_line++; }//出库训左线，采样后的左边线小于20cm，丢线计算
		if (rpts1s_num > 1.0 / sample_dist && NO_right_line > 2)//先丢线后有线
		{
			GOT_right_line++;//
		}
		// 不入库，通过编码器使得小车走过车库后才退出车库模式
		if (GOT_right_line > 1)
		{
			Garage_Type = GARAGE_NONE;
		}
		break;
	default:
		(void)0;
	}
}