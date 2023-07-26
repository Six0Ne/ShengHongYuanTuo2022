/*********************************************************************************************************************
 * File name  : corss.cpp-十字元素
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
enum cross_type_e Cross_Type = CROSS_NONE;

bool far_Lpt0_found, far_Lpt1_found;
int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;

// 以下定义为十字寻远线设定
int far_ipts0[FAR_POINTS_MAX_LEN][2];
int far_ipts1[FAR_POINTS_MAX_LEN][2];
int far_ipts0_num, far_ipts1_num;

float far_rpts0[FAR_POINTS_MAX_LEN][2];
float far_rpts1[FAR_POINTS_MAX_LEN][2];
int far_rpts0_num, far_rpts1_num;

float far_rpts0b[FAR_POINTS_MAX_LEN][2];
float far_rpts1b[FAR_POINTS_MAX_LEN][2];
int far_rpts0b_num, far_rpts1b_num;

float far_rpts0s[FAR_POINTS_MAX_LEN][2];
float far_rpts1s[FAR_POINTS_MAX_LEN][2];
int far_rpts0s_num, far_rpts1s_num;

float far_rpts0a[FAR_POINTS_MAX_LEN];
float far_rpts1a[FAR_POINTS_MAX_LEN];
int far_rpts0a_num, far_rpts1a_num;

float far_rpts0an[FAR_POINTS_MAX_LEN];
float far_rpts1an[FAR_POINTS_MAX_LEN];
int far_rpts0an_num, far_rpts1an_num;

int not_have_line = 0;



//双L角点,切十字模式
void Check_Cross()
{
	bool Xfound = Lpt0_found && Lpt1_found;//符合双角点，切换为十字模式
	if (Cross_Type == CROSS_NONE && Xfound) Cross_Type = CROSS_BEGIN;
}

void Run_Cross()
{
	buzzer = true;//执行元素，蜂鸣器叫
	bool Xfound = Lpt0_found && Lpt1_found;
	/* int64_t current_encoder = get_total_encoder();*/
	int64_t current_encoder = 0;//没用上
	float Lpt0y = rpts0s[Lpt0_rpts0s_id][1];
	float Lpt1y = rpts1s[Lpt1_rpts1s_id][1];
	//检测到十字，先按照近线走
	if (Cross_Type == CROSS_BEGIN)
	{
		if (Lpt0_found)
		{
			rpts0s_num = Lpt0_rpts0s_id;//把线截断到L角点处
			rptsc0_num = MAX(2, rpts0s_num - 3);
		}
		if (Lpt1_found)
		{
			rpts1s_num = Lpt1_rpts1s_id;
			rptsc1_num = MAX(2, rpts1s_num - 3);//把线截断到L角点处
		}

		//aim_distance = 0.40;//修改预瞄点距离

		//近角点过少，进入远线控制
		if ((Xfound && (Lpt0_rpts0s_id < 0.25 / sample_dist || Lpt1_rpts1s_id < 0.25 / sample_dist))/* || (rpts1_num <30 && rpts0_num<30)*/)
		{
			Cross_Type = CROSS_IN;//切换为进入十字模式
			//not_have_line = 0;
			//cross_encoder = current_encoder;//没用上
		}
	}
	//远线控制进十字,begin_y渐变靠近防丢线
	else if (Cross_Type == CROSS_IN)
	{
		//aim_distance = 0.45;//修改预瞄点距离
		//寻远线优化版
		Cross_FarLine2();

		if (rpts1s_num < 5 && rpts0s_num < 5) { not_have_line++; }
		if (not_have_line > 2 && rpts1s_num > 20 && rpts0s_num > 20)
		{
			Cross_Type = CROSS_NONE;
			not_have_line = 0;
		}
		if (far_Lpt1_found) { Track_Type = TRACK_RIGHT; }
		else if (far_Lpt0_found) { Track_Type = TRACK_LEFT; }
		else if (not_have_line > 0 && rpts1s_num < 5) { Track_Type = TRACK_RIGHT; }
		else if (not_have_line > 0 && rpts0s_num < 5) { Track_Type = TRACK_LEFT; }

	}
}

int far_x1 = 60, far_x2 = 260, far_y1, far_y2;

void Cross_FarLine() //十字远线
{
	int cross_width = 4;
	//far_x1 = cross_width, far_x2 = img_raw.width -cross_width;
	far_y1 = 0, far_y2 = 0;
	int x1 = image_src.cols / 2 - begin_x, y1 = begin_y;
	bool white_found = false;
	far_ipts0_num = sizeof(far_ipts0) / sizeof(far_ipts0[0]);

	//在begin_y向两边找黑线
//    for(;x1>cross_width*2; x1--) 
//    {
//      if(AT_IMAGE(&img_raw, x1-1, y1) < low_thres) 
// {
//        far_x1 = x1 - cross_width;
//        break;
//      }   
//    }
	//全白  far_x1 = 0,从边界找
	for (; y1 > 0; y1--) //爬边起始点（得把爬边起始Y用线画出来）开始往前找,thres自行更改为动态的不知道会不会有影响
	{
		//先黑后白，先找white
		if (image_src.at<uchar>(y1, far_x1) >= thres)  white_found = true; //定死的点是黑的，然后往上找白点，一直找
		if (image_src.at<uchar>(y1, far_x1) < thres && (white_found || far_x1 == cross_width))
		{
			far_y1 = y1;
			break;
		}
	}

	//从找到角点位置开始寻找
	if (image_src.at<uchar>(far_y1 + 1, far_x1) >= thres)
		FindLine_LefThand_Mat(image_src, block_size, clip_value, far_x1, far_y1 + 1, far_ipts0, &far_ipts0_num);
	else far_ipts0_num = 0;

	int x2 = image_src.cols / 2 + begin_x, y2 = begin_y;
	white_found = false;
	far_ipts1_num = sizeof(far_ipts1) / sizeof(far_ipts1[0]);

	//在begin_y向两边找黑线
	//    for(;x2<img_raw.width-cross_width*2; x2++) 
	//    {
	//      if(AT_IMAGE(&img_raw, x2+1, y2) < low_thres) 
	//     {
	//        far_x2 = x2 + cross_width;
	//        break;
	//      }   
	//    }
		//全白  far_x2 = 0,从边界找
	for (; y2 > 0; y2--)
	{
		//先黑后白，先找white
		if (image_src.at<uchar>(y2, far_x2) >= thres) { white_found = true; }
		if (image_src.at<uchar>(y2, far_x2) < thres && (white_found || far_x2 == image_src.cols - cross_width))
		{
			far_y2 = y2;
			break;
		}
	}
	//从找到像素值落差点位置开始寻找
	if (image_src.at<uchar>(far_y2 + 1, far_x2) >= thres)
		FindLine_RighThand_Mat(image_src, block_size, clip_value, far_x2, far_y2 + 1, far_ipts1, &far_ipts1_num);//右手法则开始爬没毛病
	else far_ipts1_num = 0;

	// 去畸变+透视变换
	for (int i = 0; i < far_ipts0_num; i++)
	{
		far_rpts0[i][0] = mapx[far_ipts0[i][1]][far_ipts0[i][0]];
		far_rpts0[i][1] = mapy[far_ipts0[i][1]][far_ipts0[i][0]];
	}
	far_rpts0_num = far_ipts0_num;
	for (int i = 0; i < far_ipts1_num; i++)
	{
		far_rpts1[i][0] = mapx[far_ipts1[i][1]][far_ipts1[i][0]];
		far_rpts1[i][1] = mapy[far_ipts1[i][1]][far_ipts1[i][0]];
	}
	far_rpts1_num = far_ipts1_num;

	// 边线滤波
	Blur_Points(far_rpts0, far_rpts0_num, far_rpts0b, (int)round(line_blur_kernel));
	far_rpts0b_num = far_rpts0_num;
	Blur_Points(far_rpts1, far_rpts1_num, far_rpts1b, (int)round(line_blur_kernel));
	far_rpts1b_num = far_rpts1_num;

	// 边线等距采样
	far_rpts0s_num = sizeof(far_rpts0s) / sizeof(far_rpts0s[0]);
	Resample_Points2(far_rpts0b, far_rpts0b_num, far_rpts0s, &far_rpts0s_num, sample_dist * pixel_per_meter);
	far_rpts1s_num = sizeof(far_rpts1s) / sizeof(far_rpts1s[0]);
	Resample_Points2(far_rpts1b, far_rpts1b_num, far_rpts1s, &far_rpts1s_num, sample_dist * pixel_per_meter);

	// 边线局部角度变化率
	Local_Angle_Points(far_rpts0s, far_rpts0s_num, far_rpts0a, (int)round(angle_dist / sample_dist));
	far_rpts0a_num = far_rpts0s_num;
	Local_Angle_Points(far_rpts1s, far_rpts1s_num, far_rpts1a, (int)round(angle_dist / sample_dist));
	far_rpts1a_num = far_rpts1s_num;

	// 角度变化率非极大抑制
	Nms_Angle(far_rpts0a, far_rpts0a_num, far_rpts0an, (int)round(angle_dist / sample_dist) * 2 + 1);
	far_rpts0an_num = far_rpts0a_num;
	Nms_Angle(far_rpts1a, far_rpts1a_num, far_rpts1an, (int)round(angle_dist / sample_dist) * 2 + 1);
	far_rpts1an_num = far_rpts1a_num;

	// 找远线上的L角点
	far_Lpt0_found = far_Lpt1_found = false;
	for (int i = 0; i < MIN(far_rpts0s_num, 80); i++)
	{
		if (far_rpts0an[i] == 0) continue;
		int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, far_rpts0s_num - 1);
		int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, far_rpts0s_num - 1);
		float conf = fabs(far_rpts0a[i]) - (fabs(far_rpts0a[im1]) + fabs(far_rpts0a[ip1])) / 2;
		if (70. / 180. * PI < conf && conf < 100. / 180. * PI && i < 100)
		{
			far_Lpt0_rpts0s_id = i;
			far_Lpt0_found = true;
			break;
		}
	}
	for (int i = 0; i < MIN(far_rpts1s_num, 80); i++)
	{
		if (far_rpts1an[i] == 0) continue;
		int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, far_rpts1s_num - 1);
		int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, far_rpts1s_num - 1);
		float conf = fabs(far_rpts1a[i]) - (fabs(far_rpts1a[im1]) + fabs(far_rpts1a[ip1])) / 2;

		if (70. / 180. * PI < conf && conf < 100. / 180. * PI && i < 100)
		{
			far_Lpt1_rpts1s_id = i;
			far_Lpt1_found = true;
			break;
		}
	}
}

float(*findfar_rpts)[2];
int findfar_rpts_num;
void Cross_FarLine2() //十字远线2.0优化版，利用左右边线寻找远角点
{
	int cross_width = 4;
	//far_x1 = cross_width, far_x2 = img_raw.width -cross_width;
	int x1, y1;
	int pt[2];
	if (Lpt0_found&&map_inv(rpts0s[Lpt0_rpts0s_id], pt))
	{
		far_x1 = pt[0] - 10, y1 = pt[1] - 2;
	}
	else
	{
		far_x1 = 60, y1 = begin_y;
	}
	far_y1 = 0, far_y2 = 0;

	bool white_found = false;
	far_ipts0_num = sizeof(far_ipts0) / sizeof(far_ipts0[0]);


	for (; y1 > 0; y1--) //爬边起始点（得把爬边起始Y用线画出来）开始往前找,thres自行更改为动态的不知道会不会有影响
	{
		//先黑后白，先找white
		if (image_src.at<uchar>(y1, far_x1) >= thres)  white_found = true; //定死的点是黑的，然后往上找白点，一直找
		if (image_src.at<uchar>(y1, far_x1) < thres && white_found)
		{
			far_y1 = y1;
			break;
		}
	}

	//从找到角点位置开始寻找
	if (image_src.at<uchar>(far_y1 + 1, far_x1) >= thres)
		FindLine_LefThand_Mat(image_src, block_size, clip_value, far_x1, far_y1 + 1, far_ipts0, &far_ipts0_num);
	else far_ipts0_num = 0;

	int x2, y2;
	if (Lpt1_found&&map_inv(rpts1s[Lpt1_rpts1s_id], pt))//并且边线数量正常
	{
		far_x2 = pt[0] + 10, y2 = pt[1] - 2;
	}
	else
	{
		far_x2 = 260, y2 = begin_y;
	}
	white_found = false;
	far_ipts1_num = sizeof(far_ipts1) / sizeof(far_ipts1[0]);

	//在begin_y向两边找黑线
	//    for(;x2<img_raw.width-cross_width*2; x2++) 
	//    {
	//      if(AT_IMAGE(&img_raw, x2+1, y2) < low_thres) 
	//     {
	//        far_x2 = x2 + cross_width;
	//        break;
	//      }   
	//    }
		//全白  far_x2 = 0,从边界找
	for (; y2 > 0; y2--)
	{
		//先黑后白，先找white
		if (image_src.at<uchar>(y2, far_x2) >= thres) { white_found = true; }
		if (image_src.at<uchar>(y2, far_x2) < thres && white_found)
		{
			far_y2 = y2;
			break;
		}
	}
	//从找到像素值落差点位置开始寻找
	if (image_src.at<uchar>(far_y2 + 1, far_x2) >= thres)
		FindLine_RighThand_Mat(image_src, block_size, clip_value, far_x2, far_y2 + 1, far_ipts1, &far_ipts1_num);//右手法则开始爬没毛病
	else far_ipts1_num = 0;

	// 去畸变+透视变换
	for (int i = 0; i < far_ipts0_num; i++)
	{
		far_rpts0[i][0] = mapx[far_ipts0[i][1]][far_ipts0[i][0]];
		far_rpts0[i][1] = mapy[far_ipts0[i][1]][far_ipts0[i][0]];
	}
	far_rpts0_num = far_ipts0_num;
	for (int i = 0; i < far_ipts1_num; i++)
	{
		far_rpts1[i][0] = mapx[far_ipts1[i][1]][far_ipts1[i][0]];
		far_rpts1[i][1] = mapy[far_ipts1[i][1]][far_ipts1[i][0]];
	}
	far_rpts1_num = far_ipts1_num;

	// 边线滤波
	Blur_Points(far_rpts0, far_rpts0_num, far_rpts0b, (int)round(line_blur_kernel));
	far_rpts0b_num = far_rpts0_num;
	Blur_Points(far_rpts1, far_rpts1_num, far_rpts1b, (int)round(line_blur_kernel));
	far_rpts1b_num = far_rpts1_num;

	// 边线等距采样
	far_rpts0s_num = sizeof(far_rpts0s) / sizeof(far_rpts0s[0]);
	Resample_Points2(far_rpts0b, far_rpts0b_num, far_rpts0s, &far_rpts0s_num, sample_dist * pixel_per_meter);
	far_rpts1s_num = sizeof(far_rpts1s) / sizeof(far_rpts1s[0]);
	Resample_Points2(far_rpts1b, far_rpts1b_num, far_rpts1s, &far_rpts1s_num, sample_dist * pixel_per_meter);

	// 边线局部角度变化率
	Local_Angle_Points(far_rpts0s, far_rpts0s_num, far_rpts0a, (int)round(angle_dist / sample_dist));
	far_rpts0a_num = far_rpts0s_num;
	Local_Angle_Points(far_rpts1s, far_rpts1s_num, far_rpts1a, (int)round(angle_dist / sample_dist));
	far_rpts1a_num = far_rpts1s_num;

	// 角度变化率非极大抑制
	Nms_Angle(far_rpts0a, far_rpts0a_num, far_rpts0an, (int)round(angle_dist / sample_dist) * 2 + 1);
	far_rpts0an_num = far_rpts0a_num;
	Nms_Angle(far_rpts1a, far_rpts1a_num, far_rpts1an, (int)round(angle_dist / sample_dist) * 2 + 1);
	far_rpts1an_num = far_rpts1a_num;

	// 找远线上的L角点
	far_Lpt0_found = far_Lpt1_found = false;
	for (int i = 0; i < MIN(far_rpts0s_num, 80); i++)
	{
		if (far_rpts0an[i] == 0) continue;
		int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, far_rpts0s_num - 1);
		int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, far_rpts0s_num - 1);
		float conf = fabs(far_rpts0a[i]) - (fabs(far_rpts0a[im1]) + fabs(far_rpts0a[ip1])) / 2;
		if (70. / 180. * PI < conf && conf < 100. / 180. * PI && i < 100)
		{
			far_Lpt0_rpts0s_id = i;
			far_Lpt0_found = true;
			break;
		}
	}
	for (int i = 0; i < MIN(far_rpts1s_num, 80); i++)
	{
		if (far_rpts1an[i] == 0) continue;
		int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, far_rpts1s_num - 1);
		int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, far_rpts1s_num - 1);
		float conf = fabs(far_rpts1a[i]) - (fabs(far_rpts1a[im1]) + fabs(far_rpts1a[ip1])) / 2;

		if (70. / 180. * PI < conf && conf < 100. / 180. * PI && i < 100)
		{
			far_Lpt1_rpts1s_id = i;
			far_Lpt1_found = true;
			break;
		}
	}
}
