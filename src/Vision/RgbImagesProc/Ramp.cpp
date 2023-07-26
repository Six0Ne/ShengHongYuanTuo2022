/*********************************************************************************************************************
 * File name  : Rmap.cpp-坡道元素
 * Target core: EB FZ3B
 *  Created on: 2021年8月3日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
enum ramp_type_e Ramp_Type = RAMP_NONE;

// 压缩后灰原图左右边线
int ramp0[RAMP_POINTS_MAX_LEN][2];//左线
int ramp1[RAMP_POINTS_MAX_LEN][2];//右线
int ramp_mid[RAMP_POINTS_MAX_LEN][2];//右线
int ramp0_num, ramp1_num, ramp_mid_num;//左右边线像素点数
//图像爬边参数
float ramp_thres = 125;//起始行阈值
float ramp_block_size = 3;//根据分辨率大小调整
float ramp_clip_value = 2;
float ramp_begin_x = 30;//边线爬取的起始X，和跟踪起始点不太一样
float ramp_begin_y = 80;//边线爬取的起始Y，和跟踪起始点不太一样
float ramp_cx =Rize_Cols/2 +3,ramp_cy=Rize_Rows*0.84;

void Ramp_EdgeLine(Mat& rampimg)//坡道控制模式下视觉获取边线
{
	ramp_thres = MyOSTU(rampimg, ramp_begin_y, rampimg.cols);//大津法获取起始行阈值，起始行靠近车会比较稳定
	int x1 = rampimg.cols / 2 - ramp_begin_x, y1 = ramp_begin_y;
	ramp0_num = sizeof(ramp0) / sizeof(ramp0[0]);
	for (; x1 > 0; x1--) if (rampimg.at<uchar>(y1, x1 - 1) < ramp_thres) break;
	if (rampimg.at<uchar>(y1, x1) >= ramp_thres)//往左遍历得到起始行黑白跳变点，往上爬取边线
		FindLine_LefThand_Mat(rampimg, ramp_block_size, ramp_clip_value, x1, y1, ramp0, &ramp0_num);
	else ramp0_num = 0;
	int x2 = rampimg.cols / 2 + ramp_begin_x - 2, y2 = ramp_begin_y;
	ramp1_num = sizeof(ramp1) / sizeof(ramp1[0]);
	for (; x2 < rampimg.cols - 1; x2++) if (rampimg.at<uchar>(y2, x2 + 1) < ramp_thres) break;
	if (rampimg.at<uchar>(y2, x2) >= thres)//往右遍历得到起始行黑白跳变点，往上爬取边线
		FindLine_RighThand_Mat(rampimg, ramp_block_size, ramp_clip_value, x2, y2, ramp1, &ramp1_num);
	else ramp1_num = 0;

}
void Ramp_MidLine(int pts_in0[][2], int pts_in1[][2], int pts_out[][2], int num)//简单拟合中线，控制坡道不需要太精准的中线
{
	for (int i = 0; i < num; i++)
	{
		pts_out[i][0] = (pts_in0[i][0] + pts_in1[i][0]) / 2;
		pts_out[i][1] = (pts_in0[i][1] + pts_in1[i][1]) / 2;
	}
}
int aim_idx_ramp = 15;
float Ramp_Control(int pts_in0[][2])//坡道控制部分
{
	float error_ramp =  ramp_cx-pts_in0[aim_idx_ramp][0] ;
	// float dy_ramp = ramp_cy - pts_in0[aim_idx_ramp][1];
	// float dn_ramp = sqrt(dx_ramp * dx_ramp + dy_ramp * dy_ramp);
	// float error_ramp = -atan2f(dx_ramp, dy_ramp) * 180 / PI;
	//printf("error_ramp = %f\n", error_ramp);
	return error_ramp;
}
Mat ramp_image_rgb;
void Ramp_Proc(Mat& img0)
{
	Rgb2Resize_proc(img0, ramp_image_rgb, Rize_Cols, Rize_Rows);//图像压缩
	Rgb2Gray(ramp_image_rgb, resize_garyimage);
	Ramp_EdgeLine(resize_garyimage);
	ramp_mid_num = ramp0_num < ramp1_num ? ramp0_num : ramp1_num;
	Ramp_MidLine(ramp0, ramp1, ramp_mid, ramp_mid_num);
	rgb_servo = (0.35*Ramp_Control(ramp_mid))*100;
	//Ramp_Disp(ramp_image_rgb);
	

}
//坡道状态机
int Ramp_EulerAngle = 0;
void Chrek_Ramp(void)
{
	if (Ramp_Type == RAMP_NONE && TrackRedCla.Ramp_Num > 10)
	{
		Ramp_Proc(Camera0);
		if(ramp0_num > 10 && ramp1_num >10)
		{
			Ramp_Type = RAMP_FOUND;
			Ramp_EulerAngle = TC264.eulerAngle;//发现坡道记录陀螺仪值
		}
		
	}
}
int ramp_cnt = 0;

void Run_Ramp(void)
{
	buzzer = true;//执行元素，蜂鸣器叫
	Ramp_Proc(Camera0);//坡道视觉和控制处理
	if (Ramp_Type == RAMP_FOUND)
	{
		if (TC264.eulerAngle - Ramp_EulerAngle  < -10)
		{
			Ramp_Type = RAMP_IN;
		}
	}
	else if (Ramp_Type == RAMP_IN)
	{
		if (TC264.eulerAngle - Ramp_EulerAngle > 10)
		{
			Ramp_Type = RAMP_OUT;
		}
	}
	else if (Ramp_Type == RAMP_OUT)
	{
		if (TC264.eulerAngle - Ramp_EulerAngle <10  && TC264.eulerAngle - Ramp_EulerAngle >-10)
		{
			Ramp_Type = RAMP_NONE;
			TrackRedCla.ClearALL();
		}
	}
}
//坡道视觉显示
void Ramp_Disp(Mat& DISP)
{
	for (unsigned char i = 0; i < ramp0_num; i++)
	{
		circle(DISP, Point(ramp0[i][0], ramp0[i][1]), 0.1, Scalar(0, 255, 0), -1);
	}
	for (unsigned char i = 0; i < ramp1_num; i++)
	{
		circle(DISP, Point(ramp1[i][0], ramp1[i][1]), 0.1, Scalar(0, 255, 255), -1);
		// cout << "ramppts0[i][0] = " << ramppts0[i][0] << endl;
		// cout << "ramppts0[i][1] = " << ramppts0[i][1] << endl;
	}
	for (unsigned char i = 0; i < ramp_mid_num; i++)
	{
		circle(DISP, Point(ramp_mid[i][0], ramp_mid[i][1]), 0.1, Scalar(255, 0, 255), -1);
		// cout << "ramppts0[i][0] = " << ramppts0[i][0] << endl;
		// cout << "ramppts0[i][1] = " << ramppts0[i][1] << endl;
	}
	line(DISP, Point(0, ramp_begin_y), Point(DISP.size().width / 2 - ramp_begin_x, ramp_begin_y), Scalar(0, 0, 0));
	line(DISP, Point(DISP.size().width / 2 + ramp_begin_x, ramp_begin_y), Point(DISP.size().width, ramp_begin_y), Scalar(0, 0, 0));
	//circle(DISP, Point(INPoint[1], INPoint[0]), 0.1, Scalar(255, 255, 255), -1);	
	imshow("RGB_DISP", DISP);
	waitKey(1);
}