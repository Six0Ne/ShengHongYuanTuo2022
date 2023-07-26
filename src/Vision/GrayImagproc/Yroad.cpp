#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
//0628 出泛行区仍然还是有BUG

enum yroad_type_e Yroad_Type = YROAD_NONE;

bool generic_Ypt0_found, generic_Ypt1_found;
int generic_Ypt0_rpts0s_id, generic_Ypt1_rpts1s_id;
//进入泛型区远线设定
int faryroad_ipts1[Generic_POINTS_MAX_LEN][2];//出泛型区找的远线
int faryroad_ipts1_num;

float faryroad_rpts1[Generic_POINTS_MAX_LEN][2];
int faryroad_rpts1_num;

float faryroad_rpts1b[Generic_POINTS_MAX_LEN][2];
int faryroad_rpts1b_num;

float faryroad_rpts1s[Generic_POINTS_MAX_LEN][2];
int faryroad_rpts1s_num;

int Left_Border_None_Generic = 0;
//以下定义为泛行区寻线设定
int generic_ipts0[Generic_POINTS_MAX_LEN][2];//出泛型区找的远线
int generic_ipts1[Generic_POINTS_MAX_LEN][2];//泛行区内部线
int generic_ipts0_num, generic_ipts1_num;

float generic_rpts0[Generic_POINTS_MAX_LEN][2];
float generic_rpts1[Generic_POINTS_MAX_LEN][2];
int generic_rpts0_num, generic_rpts1_num;

float generic_rpts0b[Generic_POINTS_MAX_LEN][2];
float generic_rpts1b[Generic_POINTS_MAX_LEN][2];
int generic_rpts0b_num, generic_rpts1b_num;

float generic_rpts0s[Generic_POINTS_MAX_LEN][2];
float generic_rpts1s[Generic_POINTS_MAX_LEN][2];
int generic_rpts0s_num, generic_rpts1s_num;

float generic_rpts0a[Generic_POINTS_MAX_LEN];
float generic_rpts1a[Generic_POINTS_MAX_LEN];
int generic_rpts0a_num, generic_rpts1a_num;

float generic_rpts0an[Generic_POINTS_MAX_LEN];
float generic_rpts1an[Generic_POINTS_MAX_LEN];
int generic_rpts0an_num, generic_rpts1an_num;

float(*generic_rpts)[2];
int generic_rpts_num;


void Check_Yroad()
{
	bool Yfound = Ypt0_found && Ypt1_found;
	if (Yroad_Type == YROAD_NONE && Yfound) Yroad_Type = YROAD_FOUND;
}

void Run_Yroad()
{
	buzzer = true;//执行元素，蜂鸣器叫
	Track_Type == TRACK_RIGHT;//切换巡线模式
	bool Yfound = Ypt0_found && Ypt1_found;
	bool generic_get_0 = false, generic_get_1 = false;
	// // 状态切换
	if (Yroad_Type == YROAD_FOUND)//判到角点
	{
		aim_distance = 0.40;
		generic_Ypt1_rpts1s_id = 0;//清空赛道内部的角点
		Track_Type = TRACK_RIGHT;
		if (Ypt0_found)
		{
			rpts0s_num = Ypt0_rpts0s_id;
			rptsc0_num = MAX(2, rpts0s_num - 3);//把线截断到Y角点处
		}
		if (Ypt1_found)
		{
			rpts1s_num = Ypt1_rpts1s_id;
			rptsc1_num = MAX(2, rpts1s_num - 3);//把线截断到Y角点处
			//cout << "rptsc1_num = " << rptsc1_num << endl;            
		}
		//cout << "generic_Ypt1_rpts1s_id = " << Ypt1_rpts1s_id << endl;
		if (Yfound && (Ypt0_rpts0s_id < 0.30 / sample_dist || Ypt1_rpts1s_id < 0.30 / sample_dist))
		{
			Left_Border_None_Generic = 0;
			generic_Ypt1_rpts1s_id = 0;
			Yroad_Type = YROAD_NEAR;
		}

	}
	else if (Yroad_Type == YROAD_NEAR)
	{
		// Track_Type = TRACK_RIGHT;
		aim_distance = 1.20;
		if (Ypt0_found)
		{
			rpts0s_num = Ypt0_rpts0s_id;
			rptsc0_num = MAX(2, rpts0s_num - 3);//把线截断到Y角点处
		}
		if (Ypt1_found)
		{
			rpts1s_num = Ypt1_rpts1s_id;
			rptsc1_num = MAX(2, rpts1s_num - 3);//把线截断到Y角点处
			//cout << "rptsc1_num = " << rptsc1_num << endl;            
		}
		FarYroad_Line();
		int generic_x0 = image_src.cols / 2 - begin_x, generic_y0 = begin_y;
		generic_get_0 = image_src.at<uchar>(generic_y0, generic_x0) < thres;//左边找线起始点为黑

		int generic_x1 = image_src.cols / 2 + begin_x, generic_y1 = begin_y;
		generic_get_1 = image_src.at<uchar>(generic_y1, generic_x1) < thres;//右边找线起始点为黑

		bool Gfound = generic_get_0 && generic_get_1;//两个起始点都为黑，进入泛行区
		if (Gfound) 
		{
			Left_Border_None_Generic = 0;
			generic_Ypt1_rpts1s_id = 0;
			Yroad_Type = GENERIC_IN;
		}
	}
	else if (Yroad_Type == GENERIC_IN)
	{
		//角点继续截断
		Left_Border_None_Generic = 0;
		Track_Type == TRACK_RIGHT;//切换巡线模式
		aim_distance = 0.65;
		Generic_InLine();//泛行区找线，目前只找右线
		 //cout << "generic_Ypt1_rpts1s_id = " << generic_Ypt1_rpts1s_id << endl;
		if (generic_Ypt1_found)
		{
			generic_rpts1s_num = generic_Ypt1_rpts1s_id;//把边线线截断到Y角点处

			//cout << "generic_Ypt1_rpts1s_id = " << generic_Ypt1_rpts1s_id << endl;
		}
		if (generic_Ypt1_rpts1s_id > 3 && generic_Ypt1_rpts1s_id < 0.90 / sample_dist)//明天排查一下
		{
			//generic_outline();//查找 出 泛行区线
			Yroad_Type = GENERIC_OUT;
		}

	}
	else if (Yroad_Type == GENERIC_OUT)
	{
		aim_distance = 1.40;
		// generic_inline();//
		// if(generic_Ypt1_found)
		// {
		//     generic_rpts1s_num = generic_Ypt1_rpts1s_id;//把边线线截断到Y角点处

		//     cout << "generic_Ypt1_rpts1s_id = " << generic_Ypt1_rpts1s_id << endl;
		// }//继续截断
		Generic_InLine();
		Generic_OutLine();//查找出泛行区线
		int generic_outx1 = image_src.cols / 2 + begin_x, generic_outy1 = begin_y;
		int generic_outx2 = image_src.cols / 2 - begin_x, generic_outy2 = begin_y;
		if (image_src.at<uchar>(generic_outy1, generic_outx1 + 1) > thres && image_src.at<uchar>(generic_outy2, generic_outx2 - 1) > thres)
		{
			if ( rpts0s_num < 0.1 / sample_dist)//防止误判 驶过两个角点过再清除
			{
				Left_Border_None_Generic++;
				if(Left_Border_None_Generic > 1)
				{
					Yroad_Type = YROAD_NONE;
				//track_type = TRACK_LEFT;//继续寻左线
					generic_Ypt1_found = false;//角点判断标志位清空
					generic_rpts1s_num = 0;//清空赛道内部线
					generic_Ypt1_rpts1s_id = 0;//清空赛道内部的角点
				}
			}
		}


	}
}
void FarYroad_Line() //三叉扫远线，进入泛行区
{
	int Lpt[2];
	int Rpt[2];
	int x1, y1;
	int farY_x1 = 260, farY_y1 = 0;
	if (map_inv(rpts0s[Ypt0_rpts0s_id], Lpt) && map_inv(rpts1s[Ypt1_rpts1s_id], Rpt))
	{
		farY_x1 = (Lpt[0] + Rpt[0]) / 2; 
		y1 = (Lpt[1] + Rpt[1]) / 2;
	}
	else
	{
		farY_x1 = image_src.cols / 2 + begin_x;
		y1 = begin_y - 1;
	}

	bool fywhite_found = false;
	faryroad_ipts1_num = sizeof(faryroad_ipts1) / sizeof(faryroad_ipts1[0]);

	for (; y1 > 0; y1--)
	{
		//先黑后白，先找white
		if (image_src.at<uchar>(y1, farY_x1) >= thres) { fywhite_found = true; }
		if (image_src.at<uchar>(y1, farY_x1) < thres && fywhite_found)
		{
			farY_y1 = y1;
			break;
		}
	}
	//从找到像素值落差点位置开始寻找
	if (image_src.at<uchar>(farY_y1 + 1, farY_x1) >= thres)
		FindLine_LefThand_Mat(image_src, block_size, clip_value, farY_x1, farY_y1 + 1, faryroad_ipts1, &faryroad_ipts1_num);//右手法则开始爬没毛病
	else faryroad_ipts1_num = 0;
	//cout << "faryroad_ipts1_num = " << faryroad_ipts1_num << endl;
	//矫正+透视逆变换
	for (int i = 0; i < faryroad_ipts1_num; i++)
	{
		faryroad_rpts1[i][0] = mapx[faryroad_ipts1[i][1]][faryroad_ipts1[i][0]];
		faryroad_rpts1[i][1] = mapy[faryroad_ipts1[i][1]][faryroad_ipts1[i][0]];
	}
	faryroad_rpts1_num = faryroad_ipts1_num;

	// 边线滤波
	Blur_Points(faryroad_rpts1, faryroad_rpts1_num, faryroad_rpts1b, (int)round(line_blur_kernel));
	faryroad_rpts1b_num = faryroad_rpts1_num;

	// 边线等距采样
	faryroad_rpts1s_num = sizeof(faryroad_rpts1s) / sizeof(faryroad_rpts1s[0]);
	Resample_Points2(faryroad_rpts1b, faryroad_rpts1b_num, faryroad_rpts1s, &faryroad_rpts1s_num, sample_dist * pixel_per_meter);

}
void Generic_InLine() //泛行区扫线，寻右泛行区
{
	int generic_x1 = image_src.cols / 2 + begin_x, generic_y1 = begin_y;
	generic_ipts1_num = sizeof(generic_ipts1) / sizeof(generic_ipts1[0]);
	//cout << "thres = " << thres << endl;
	for (; generic_x1 < image_src.cols - 1; generic_x1++) if (image_src.at<uchar>(generic_y1, generic_x1 + 1) > thres) break;
	if (image_src.at<uchar>(generic_y1, generic_x1) <= thres)
		FindLine_RighThand_Bluecloth(image_src, block_size, clip_value, generic_x1, generic_y1, generic_ipts1, &generic_ipts1_num);
	else generic_ipts1_num = 0;
	//cout << "generic_x1 = " << generic_x1 << endl;
   //矫正+透视逆变换
	for (int i = 0; i < generic_ipts1_num; i++)
	{
		generic_rpts1[i][0] = mapx[generic_ipts1[i][1]][generic_ipts1[i][0]];
		generic_rpts1[i][1] = mapy[generic_ipts1[i][1]][generic_ipts1[i][0]];
	}
	generic_rpts1_num = generic_ipts1_num;

	// 边线滤波
	Blur_Points(generic_rpts1, generic_rpts1_num, generic_rpts1b, (int)round(line_blur_kernel));
	generic_rpts1b_num = generic_rpts1_num;

	// 边线等距采样
	generic_rpts1s_num = sizeof(generic_rpts1s) / sizeof(generic_rpts1s[0]);
	Resample_Points2(generic_rpts1b, generic_rpts1b_num, generic_rpts1s, &generic_rpts1s_num, sample_dist * pixel_per_meter);

	// 边线局部角度变化率
	Local_Angle_Points(generic_rpts1s, generic_rpts1s_num, generic_rpts1a, (int)round(angle_dist / sample_dist));
	generic_rpts1a_num = generic_rpts1s_num;

	// 角度变化率非极大抑制
	Nms_Angle(generic_rpts1a, generic_rpts1a_num, generic_rpts1an, (int)round(angle_dist / sample_dist) * 2 + 1);
	generic_rpts1an_num = generic_rpts1a_num;

	// 
	generic_Ypt1_found = false;
	for (int i = 0; i < MIN(generic_rpts1s_num, 100); i++)
	{
		if (generic_rpts1an[i] == 0) continue;
		int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, generic_rpts1s_num - 1);
		int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, generic_rpts1s_num - 1);
		float conf = fabs(generic_rpts1a[i]) - (fabs(generic_rpts1a[im1]) + fabs(generic_rpts1a[ip1])) / 2;

		if (50. / 180. * PI < conf && conf < 65. / 180. * PI && i < 100)
		{
			generic_Ypt1_rpts1s_id = i;
			if (generic_rpts1s[generic_Ypt1_rpts1s_id][i] < generic_rpts1s[generic_rpts1s_num][i])
			{
				generic_Ypt1_found = true;
			}
			else 
			{
				generic_Ypt1_found = false;
			}
			break;
		}
	}
}

void Generic_OutLine() //出泛行区扫线，寻左泛行区
{

	int ypt[2];
	int genericfar_y0 = 0;
	bool gengeneric_white = false;
	int generic_x0 = image_src.cols / 2 - begin_x - 20, generic_y0 = begin_y;
	if (map_inv(generic_rpts1s[generic_Ypt1_rpts1s_id], ypt))
	{
		generic_x0 = ypt[0] - 30, generic_y0 = ypt[1] - 5;
	}
	else
	{
		generic_x0 = image_src.cols / 2 - begin_x - 30, generic_y0 = begin_y;
	}//边线角点回到原始域区找出去的边线
   // cout << "ypt[0] = " << ypt[0] << endl;
   // cout << "ypt[1] = " << ypt[1] << endl;
	// for (; ypt[0] < image_src.cols - 1; ypt[0]++) if (image_src.at<uchar>(ypt[1], ypt[0] + 1) < thres) break;
	// cout << "ypt[0] = " << ypt[0] << endl;
	// cout << "ypt[1] = " << ypt[1] << endl;


	generic_ipts0_num = sizeof(generic_ipts0) / sizeof(generic_ipts0[0]);
	for (; generic_y0 > 0; generic_y0--) //爬边起始点（得把爬边起始Y用线画出来）开始往前找,thres自行更改为动态的不知道会不会有影响
	{
		//先黑后白，先找white
		if (image_src.at<uchar>(generic_y0, generic_x0) >= thres)  gengeneric_white = true; //定死的点是黑的，然后往上找白点，一直找
		if (image_src.at<uchar>(generic_y0, generic_x0) < thres && gengeneric_white)
		{
			genericfar_y0 = generic_y0;
			break;
		}
	}
	// cout << "thres = " << thres << endl;
	 //cout << "generic_x0 = " << generic_x0 << endl;
	// cout << "generic_y0 = " << generic_y0 << endl;
	if (image_src.at<uchar>(genericfar_y0 + 1, generic_x0) >= thres)
		FindLine_LefThand_Mat(image_src, block_size, clip_value, generic_x0, genericfar_y0 + 1, generic_ipts0, &generic_ipts0_num);
	else generic_ipts0_num = 0;
	//cout << "generic_x1 = " << generic_x1 << endl;
   //矫正+透视逆变换
	for (int i = 0; i < generic_ipts0_num; i++)
	{
		generic_rpts0[i][0] = mapx[generic_ipts0[i][1]][generic_ipts0[i][0]];
		generic_rpts0[i][1] = mapy[generic_ipts0[i][1]][generic_ipts0[i][0]];
	}
	generic_rpts0_num = generic_ipts0_num;
	//cout << "generic_rpts0_num = " << generic_rpts0_num << endl;
	// 边线滤波
	Blur_Points(generic_rpts0, generic_rpts0_num, generic_rpts0b, (int)round(line_blur_kernel));
	generic_rpts0b_num = generic_rpts0_num;

	// 边线等距采样
	generic_rpts0s_num = sizeof(generic_rpts0s) / sizeof(generic_rpts0s[0]);
	Resample_Points2(generic_rpts0b, generic_rpts0b_num, generic_rpts0s, &generic_rpts0s_num, sample_dist * pixel_per_meter);
}