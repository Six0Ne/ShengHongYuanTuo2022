/*********************************************************************************************************************
 * File name  : WorkZone.cpp-施工区元素
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
enum WorkZone_type_e WorkZone_Type = WORKZONE_NONE;



int WorkZone_Enter_Cone_Left[2];
int WorkZone_Enter_Cone_Right[2];
int WorkZone_Exit_Cone[2];
int WorkZone_Exit_Point[2];

//数组点集合
vector<cv::Point> WorkZone_LeftPoints(Rize_Rows);
Vec4f WorkZone_Line_Param_Left; //二维直线类型 有四个值 前两个值代表着直线的方向 后面两个值为直线上的一点
vector<cv::Point> WorkZone_RightPoints(Rize_Rows);
Vec4f WorkZone_Line_Param_Right;
vector<cv::Point> WorkZone_MidPoints(Rize_Rows);
Vec4f WorkZone_Line_Param_Mid;
int WorkZone_BorderH;

bool WorkZone_IfFindExit = false;
bool WorkZone_IfConeNone = false;

int WorkZone_Red_Sign_Position[2]; //记录红色坐标

int WorkZone_INPoint[2];
bool WorkZone_Find_INPoint = false;

int WorkZone_OUTPoint[2];
bool WorkZone_Find_OUTPoint = false;

bool WorkZone_Chrek_OnTrack_Red(Mat &img0) //在图像范围内一半寻找赛道上的标志
{

	int y, x;
	int sign_count = 0;
	for (y = img0.rows; y > img0.rows / 2.0; y--) //从近到远扫距车一半像素点距离的区域
	{
		for (x = img0.cols / 2 - (img0.cols * 0.20); x < img0.cols / 2 + (img0.cols * 0.20); x++)
		{
			if (img0.at<uchar>(y, x) == White) //赛道上找到连续两点白点
			{
				WorkZone_Red_Sign_Position[0] = y;
				sign_count++;
			} //利用指针去访问Mat中的元素
		}
	}
	if (sign_count > 5)
	{
		return true;
	}
	WorkZone_Red_Sign_Position[0] = -1;
	WorkZone_Red_Sign_Position[1] = -1;
	return false;
}

int WorkZone_Chrek_OffTrack_Red(Mat &img1,cv::Rect rect) //检查赛道外的红色区域
{
int y, x;
	int side_count_left = 0;
	int side_count_right = 0;

	for (x = rect.tl().x; x >= 0; x--)
    {
        for (y = img1.rows - 1; y >= 0; y--)
        {
            if (img1.at<uchar>(y, x) == White)
            {
                side_count_left++;
                break;
            }
        }
    }
	if (side_count_left > 3)// 这个系数再调一下
    {
        return 1;
    }

    /* 检测右边 */
    for (x = rect.br().x; x < img1.cols; x++)
    {
        for (y = img1.rows - 1; y >= 0; y--)
        {
            if (img1.at<uchar>(y, x) == White)
            {
                side_count_right++;
                break;
            }
        }
    }
    if (side_count_right > 10)
    {
        return 2;
    }

    return 0;
}
float WorkZone_StraightLineFunction_Y(int X, cv::Vec4f Line_Param)
{
	return ((Line_Param[1] / Line_Param[0]) * (X - Line_Param[2]) + Line_Param[3]);//直线公式
}

float WorkZone_StraightLineFunction_X(int Y, cv::Vec4f Line_Param)
{
	return ((Y - Line_Param[3]) / (Line_Param[1] / Line_Param[0]) + Line_Param[2]);//直线公式
}
void WorkZone_Control()
{
	float error_rgb = 0;
	if ((WorkZone_Type == WORKZONE_LEFT_NEARDOOR))
	{
		error_rgb = Rize_Cols / 2 - (WorkZone_Enter_Cone_Left[1] + WorkZone_Enter_Cone_Right[1]) / 2;
	}
	else if (WorkZone_Type == WORKZONE_LEFT_IN)
	{
		error_rgb = Rize_Cols / 2 - WorkZone_INPoint[1];
		// cout << "servo = " << servo << endl;
	}
	else if (WorkZone_Type == WORKZONE_LEFT_RUNNING)
	{
		error_rgb = Rize_Cols / 2 - (WorkZone_StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Left) +
									 0.5 * (WorkZone_StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Right) - WorkZone_StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Left)));
		// cout << "servo = " << (StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Left) +
		//                   0.7 * (StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Right) - StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Left))) << endl;
	}
	else if (WorkZone_Type == WORKZONE_LEFT_OUT)
	{
		error_rgb = Rize_Cols / 2 - WorkZone_Exit_Point[1];
	}
	float RGB_angle = Servo_PID(&RGBPID_Servo, error_rgb);
	// float RGB_angle =0.3*error_rgb;
	// cout << "RGB_angle = " << RGB_angle << endl;
	rgb_servo = RGB_angle * 100;
}
// int cone_ipts0_num, cone_ipts1_num;
// 0626有一次出泛型区的时候出去了，还不知道是啥原因，可能是误判了
// 0626 施工区的路径很差，小圆环会有点内切；

void Check_WorkZone()
{
	if (WorkZone_Type == WORKZONE_NONE && ROIs.size() == 1 && ROIs[0].br().y > (CorpRize_Rows *0.55))
	{
	//cout << "WorkZone_Type  -->  OK! " << endl;
		switch (WorkZone_Chrek_OffTrack_Red(red_bin_image,ROIs[0]))
		{
		case 0:
		{
			WorkZone_Type = WORKZONE_NONE;
			break;
		}
		case 1:
		{
			WorkZone_Enter_Cone_Left[0] = 0;
			WorkZone_INPoint[0] = 0;
			WorkZone_Type = WORKZONE_LEFT_BEGIN;
			//motor=80;
			break;
		}
		case 2:
		{
			WorkZone_Type = WORKZONE_RIGHT_BEGIN;
			break;
		}
		default:
		{
			break;
		}
		}
	}
}
void Run_WorkZone()
{
	
	if (WorkZone_Type == WORKZONE_LEFT_BEGIN)
	{
		if (!WorkZone_Chrek_OnTrack_Red(dilate_image))
		{
			WorkZone_Type = WORKZONE_LEFT_NEARDOOR;
		}
	}
	else if (WorkZone_Type == WORKZONE_LEFT_NEARDOOR) //设置不同状态是为了优化路径，预想能有个较好的路径快速通过施工区
	{
		WorkZone_SearchEnterCone_Left(dilate_image);
		// printf("%d\n", WorkZone_Enter_Cone_Left[0]);
		// printf("%d\n", WorkZone_Enter_Cone_Left[1]);
		if (WorkZone_Enter_Cone_Left[0] >= dilate_image.rows - 1)
		{
			WorkZone_Type = WORKZONE_LEFT_IN;
		}
	}
	else if (WorkZone_Type == WORKZONE_LEFT_IN)
	{
		WorkZone_SearchINPoint_Left(dilate_image);
		// printf("%d\n", INPoint[0]);
		if (WorkZone_INPoint[0] > 5) //图像大小换了的话这里要变，如果撞到侧面锥桶可以等出口锥桶近一点再出去
		{
			WorkZone_Type = WORKZONE_LEFT_RUNNING;
			WorkZone_IfFindExit = false;
		}
	}
	else if (WorkZone_Type == WORKZONE_LEFT_RUNNING)
	{

		WorkZone_SearchMidLine_Left(dilate_image);
		if (WorkZone_IfFindExit) //利用灰头去判有没有回到赛道
		{
			WorkZone_Type = WORKZONE_LEFT_OUT;
			WorkZone_IfConeNone = false;
		}
	}
	else if (WorkZone_Type == WORKZONE_LEFT_OUT)
	{

		WorkZone_SearchOUTPoint_Left(dilate_image);
		if (WorkZone_BackToRoad(image_src)) //利用灰头去判有没有回到赛道
		{
			WorkZone_Type = WORKZONE_NONE;
		}
	}
}

void WorkZone_SearchINPoint_Left(Mat &img_bin) //可以改成关于y动态偏移X
{
	int y, x;
	for (y = 0; y < img_bin.rows; y++) // h, w起始点可以考虑重设
	{
		for (x = 0; x < img_bin.cols; x++)
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y + 1, x) == White &&
				img_bin.at<uchar>(y + 2, x) && img_bin.at<uchar>(y + 3, x))
			{
				WorkZone_INPoint[0] = y;
				WorkZone_INPoint[1] = x - 4; //动态偏移量的比例要调
				WorkZone_Find_INPoint = true;
				return;
			}
		}
	}
	WorkZone_INPoint[0] = -1;
	WorkZone_INPoint[1] = -1;
	WorkZone_Find_INPoint = false;
}

void WorkZone_SearchEnterCone_Left(Mat &img_bin) // 还没有考虑没检查到的情况
{
	int Check_Count;	// 检查点计数
	int Point_Check[2]; // 检查点缓冲区

	/* 寻找入口左下锥桶(从右往左扫，扫完一行就从下到上) */
	Check_Count = 0;							// 初始化检查点计数值
	Point_Check[0] = -1;						// 初始化检查点缓冲区H坐标
	Point_Check[1] = -1;						// 初始化检查点缓冲区W坐标
	for (int y = img_bin.rows - 1; y >= 0; y--) // 从下往上扫描
	{
		for (int x = img_bin.cols - 1; x >= 1; x--) // 从图像中线开始从右往左扫描
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y, x - 1) == White) // 如果同一行中连续两列都是白色，则认为可能是目标点
			{
				if (x <= Point_Check[1]) // 如果当前白色点的x比之前记录的x要小(或等于)，则开始进行检查
				{
					if (Check_Count == 0) // 如果是刚开始转折，则将前一个点记录为疑似目标点
					{
						WorkZone_Enter_Cone_Left[0] = Point_Check[0]; // 记录当前白色点的H坐标
						WorkZone_Enter_Cone_Left[1] = Point_Check[1]; // 记录当前白色点的W坐标
					}
					Check_Count++;		  // 检查点计数器加1
					if (Check_Count >= 3) // 如果连续三个白点都呈递减状态则退出循环
					{
						y = -1;
						break;
					}
				}
				else // 否则继续扫描记录白色点坐标
				{
					Point_Check[0] = y;
					Point_Check[1] = x;
					Check_Count = 0;
				}
			}
		}
	}

	/* 寻找入口右上锥桶(从下往上扫，扫完一行就从右到左) */
	Check_Count = 0;							// 初始化检查点计数值
	Point_Check[0] = -1;						// 初始化检查点缓冲区H坐标
	Point_Check[1] = -1;						// 初始化检查点缓冲区W坐标
	for (int x = img_bin.cols - 1; x >= 0; x--) // 从图像中线开始从右往左扫描
	{
		for (int y = img_bin.rows - 1; y >= 1; y--) // 从下往上扫描
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y - 1, x) == White) // 如果同一列中连续两行都是白色，则认为可能是目标点
			{
				if (y <= Point_Check[0]) // 如果当前白色点的H比之前记录的H要小(或等于)，则开始进行检查
				{
					if (Check_Count == 0) // 如果是刚开始转折，则将前一个点记录为疑似目标点
					{
						WorkZone_Enter_Cone_Right[0] = Point_Check[0]; // 记录当前白色点的H坐标
						WorkZone_Enter_Cone_Right[1] = Point_Check[1]; // 记录当前白色点的W坐标
					}
					Check_Count++;		  // 检查点计数器加1
					if (Check_Count >= 3) // 如果连续三个白点都呈递减状态则退出循环
					{
						x = -1;
						break;
					}
				}
				else // 否则继续扫描记录白色点坐标
				{
					Point_Check[0] = y;
					Point_Check[1] = x;
					Check_Count = 0;
				}
			}
		}
	}
}
void WorkZone_SearchMidLine_Left(Mat &img_bin)
{
	int UpPoint[2];
	int DownPoint[2];
	int y, x, x_left, x_right;

	/* 寻找施工区最上点 */
	for (int y = 0; y < img_bin.rows - 1; y++)
	{
		for (int x = 0; x < img_bin.cols / 2; x++)
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y + 1, x) == White)
			{
				UpPoint[0] = y;
				UpPoint[1] = x;
				y = img_bin.rows;
				break;
			}
		}
	}

	/* 寻找施工区最下点 */
	for (int x = 0; x < img_bin.cols / 2; x++) // 这个5要配合上面的状态切换,可以写个宏定义
	{
		for (int y = img_bin.rows - 1; y >= 1; y--)
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y - 1, x) == White)
			{
				DownPoint[0] = y;
				DownPoint[1] = x;
				x = img_bin.cols / 2;
				break;
			}
		}
	}

	/* 计算边界H */
	WorkZone_BorderH = (DownPoint[0] + UpPoint[0]) / 2;

	/* 边界H加第三象限找左边线拟合 */
	int WorkZone_TouchBorderH = 0;
	WorkZone_LeftPoints.clear();
	for (x_left = 0; x_left < img_bin.cols / 2; x_left++)
	{
		for (y = img_bin.rows - 1; y > WorkZone_BorderH; y--)
		{
			if (img_bin.at<uchar>(y, x_left) == White && img_bin.at<uchar>(y - 1, x_left) == White)
			{
				WorkZone_LeftPoints.push_back(cv::Point(x_left, y));
				break;
			}
		}
		if(y==WorkZone_BorderH)
		{
			WorkZone_TouchBorderH++;
			if (WorkZone_TouchBorderH > 2)
        	{
         	   break;
        	}
		}
	}
	
	if (WorkZone_LeftPoints.size() == 0)
    {
        WorkZone_LeftPoints.push_back(cv::Point(0, img_bin.rows / 2));
        WorkZone_LeftPoints.push_back(cv::Point(img_bin.cols / 2, 0));
    }
	cv::fitLine(WorkZone_LeftPoints, WorkZone_Line_Param_Left, cv::DIST_L2, 0, 1e-2, 1e-2);

	/* 第四象限加第一象限一个点拟合右边线 */
	WorkZone_RightPoints.clear();
	for (int y = img_bin.rows / 2; y < img_bin.rows; y++)
	{
		for (x_right = img_bin.cols * 3 / 4; x_right < img_bin.cols - 1; x_right++)
		{
			if (img_bin.at<uchar>(y, x_right) == White && img_bin.at<uchar>(y, x_right + 1) == White)
			{
				WorkZone_RightPoints.push_back(cv::Point(x_right, y));
				break;
			}
		}
	}
	if (WorkZone_RightPoints.size() == 0)
	{
		WorkZone_RightPoints.push_back(cv::Point(img_bin.cols - 1, img_bin.rows - 1));
	}
	for (int y = img_bin.rows / 2; y >= 0; y--)
	{
		for (x_right = img_bin.cols - 1; x_right > img_bin.cols / 2; x_right--)
		{
			if (img_bin.at<uchar>(y, x_right) == White && img_bin.at<uchar>(y, x_right - 1) == White)
			{
				WorkZone_RightPoints.push_back(cv::Point(x_right, y));
				x_right = img_bin.cols / 2;
				break;
			}
		}
	}
	cv::fitLine(WorkZone_RightPoints, WorkZone_Line_Param_Right, cv::DIST_L2, 0, 1e-2, 1e-2);

	/* 判断出口 */ //看第四象限上半部分是不是很少白点了(或者看最上白点H坐标)
	int WhiteCount = 0;
	for (int y = img_bin.rows / 2; y < img_bin.rows * 3 / 4; y++)
	{
		for (int x = img_bin.cols / 2; x < img_bin.cols; x++)
		{
			if (img_bin.at<uchar>(y, x) == White)
			{
				WhiteCount++;
			}
		}
	}
	if (WhiteCount < 5)
	{
		WorkZone_IfFindExit = true;
	}
}
void WorkZone_SearchOUTPoint_Left(Mat &img_bin)
{
	/* 寻找出口处锥桶 */ // 因为会变化所以不能只在第一象限(先在第一象限下半部分找，找不到了就扫第三象限)
	WorkZone_Exit_Cone[0] = -1;
	WorkZone_Exit_Cone[1] = -1;
	for (int x = img_bin.cols - 1; x >= img_bin.cols / 2; x--) // 从图像中线开始从右往左扫描
	{
		for (int y = img_bin.rows / 2; y >= img_bin.rows / 4; y--) // 从下往上扫描
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y - 1, x) == White)
			{
				WorkZone_Exit_Cone[0] = y;
				WorkZone_Exit_Cone[1] = x;
				x = -1;
				break;
			}
		}
	}

	if (WorkZone_Exit_Cone[0] == -1 && WorkZone_Exit_Cone[1] == -1)
	{
		for (int x = img_bin.cols / 2; x >= 0; x--) // 从图像中线开始从右往左扫描
		{
			for (int y = img_bin.rows - 1; y >= img_bin.rows / 2; y--) // 从下往上扫描
			{
				if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y - 1, x) == White)
				{
					WorkZone_Exit_Cone[0] = y;
					WorkZone_Exit_Cone[1] = x;
					x = -1;
					break;
				}
			}
		}
	}

	/* 对原图进行二值化 */ // 看看要不要加个啥if WorkZone_Exit_Cone[0] > 0之类的
	// 这里记得还是开个图像看看
	cv::Mat img_gra_temp;
	cv::Mat img_bin_temp;
	//这个地方可以更改一手
	cv::cvtColor(image_rgb, img_gra_temp, cv::COLOR_BGR2GRAY); // 如果还有其他地方要用的话就把这几句放到公共位置让他们只运行一次
	cv::threshold(img_gra_temp, img_bin_temp, 255, 255, cv::THRESH_OTSU);
	int x;
	bool IfFindWhite = false;
	for (x = WorkZone_Exit_Cone[1]; x < img_bin.cols - 1; x++) // 从图像中线开始从右往左扫描
	{
		if (img_bin_temp.at<uchar>(WorkZone_Exit_Cone[0], x) == White && img_bin_temp.at<uchar>(WorkZone_Exit_Cone[0], x + 1) == White)
		{ // 还是要注意一下踩着白点还是黑点
			IfFindWhite = true;
		}
		if (IfFindWhite && img_bin_temp.at<uchar>(WorkZone_Exit_Cone[0], x) == Black && img_bin_temp.at<uchar>(WorkZone_Exit_Cone[0], x + 1) == Black)
		{
			break;
		}
	}
	WorkZone_Exit_Point[0] = WorkZone_Exit_Cone[0];
	WorkZone_Exit_Point[1] = WorkZone_Exit_Cone[1] + 8;

	if (img_bin.rows / 2 <= WorkZone_Exit_Cone[0] && WorkZone_Exit_Cone[0] < img_bin.rows &&
		0 <= WorkZone_Exit_Cone[1] && WorkZone_Exit_Cone[1] < img_bin.cols / 2)
	{
		int WhiteCount = 0;
		for (int y = img_bin.rows / 4; y < img_bin.rows / 2; y++) // 第四象限下部分
		{
			for (int x = 0; x < img_bin.cols / 2; x++)
			{
				if (img_bin.at<uchar>(y, x) == White)
				{
					WhiteCount++;
				}
			}
		}
		if (WhiteCount < 5)
		{
			WorkZone_IfConeNone = true;
		}
	}
}
bool WorkZone_BackToRoad(Mat &img_raw) //感觉用灰头判有没有回到路会好一点
{

	int BackToRoad_x1 = img_raw.cols / 2 + begin_x + 5, BackToRoad_y1 = begin_y;
	int BackToRoad_x2 = img_raw.cols / 2 - begin_x - 5, BackToRoad_y2 = begin_y;
	if (image_src.at<uchar>(BackToRoad_y1, BackToRoad_x1 + 1) > thres && image_src.at<uchar>(BackToRoad_y2, BackToRoad_x2 - 1) > thres) // IfPassSign暂时拿过来用一用
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool WorkZone_Track_HaveRedSign = false;
void WorkZone_Proc(Mat &RGBCamera0)
{
	Rgb2Resize_proc(RGBCamera0, image_rgb, CorpRize_Cols, CorpRize_Rows); //图像压缩
	Rgb2LabBin_proc(image_rgb, red_bin_image);								  // HSV提取红色，后续研究看需不需要腐蚀膨胀
	WorkZone_Track_HaveRedSign = WorkZone_Chrek_OnTrack_Red(red_bin_image);
	if (WorkZone_Type == WORKZONE_NONE && WorkZone_Track_HaveRedSign && WorkZone_Red_Sign_Position[0] > red_bin_image.rows * 0.5)
	{
		Check_WorkZone();
	}
	if (WorkZone_Type != WORKZONE_NONE)
	{
		Run_WorkZone();
	}
	WorkZone_Control();
}
void WorkZone_Disp(Mat &DISP)
{
	// circle(DISP, Point(WorkZone_INPoint[1], WorkZone_INPoint[0]), 0.1, Scalar(255, 255, 255), -1);

	if (WorkZone_Type != WORKZONE_NONE)
	{
		switch (WorkZone_Type)
		{
		case WORKZONE_LEFT_BEGIN:
		{
			cv::putText(DISP, "WORKZONE_LEFT_BEGIN", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);
			break;
		}
		case WORKZONE_LEFT_NEARDOOR:
		{
			cv::putText(DISP, "WORKZONE_LEFT_NEARDOOR", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);
			int Enter_Cone_Mid[2] = {(WorkZone_Enter_Cone_Left[0] + WorkZone_Enter_Cone_Right[0]) / 2, (WorkZone_Enter_Cone_Left[1] + WorkZone_Enter_Cone_Right[1]) / 2};
			cv::circle(DISP, cv::Point(WorkZone_Enter_Cone_Left[1], WorkZone_Enter_Cone_Left[0]), 2, cv::Scalar(0, 255, 0), -1);
			cv::circle(DISP, cv::Point(WorkZone_Enter_Cone_Right[1], WorkZone_Enter_Cone_Right[0]), 2, cv::Scalar(0, 255, 0), -1);
			cv::circle(DISP, cv::Point(Enter_Cone_Mid[1], Enter_Cone_Mid[0]), 2, cv::Scalar(0, 255, 0), -1);
			break;
		}
		case WORKZONE_LEFT_IN:
		{
			cv::putText(DISP, "WORKZONE_LEFT_IN", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);
			cv::circle(DISP, cv::Point(WorkZone_INPoint[1], WorkZone_INPoint[0]), 2, cv::Scalar(0, 255, 0), -1);
			break;
		}
		case WORKZONE_LEFT_RUNNING:
		{
			cv::putText(DISP, "WORKZONE_LEFT_RUNNING", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);

			/* 寻中线上边界 */
			cv::line(DISP, cv::Point(0, WorkZone_BorderH), cv::Point(Rize_Cols, WorkZone_BorderH), cv::Scalar(255, 0, 255), 1);

			/* 左边界散点和拟合直线 */
			for (int i = 0; i < WorkZone_LeftPoints.size(); i++)
			{
				cv::circle(DISP, WorkZone_LeftPoints[i], 1, cv::Scalar(0, 255, 0), -1);
			}
			cv::line(DISP, cv::Point(0, WorkZone_StraightLineFunction_Y(0, WorkZone_Line_Param_Left)),
					 cv::Point(Rize_Cols, WorkZone_StraightLineFunction_Y(Rize_Cols, WorkZone_Line_Param_Left)), cv::Scalar(0, 255, 255), 1, 4, 0);

			/* 右边界散点和拟合直线 */
			for (int i = 0; i < WorkZone_RightPoints.size(); i++)
			{
				cv::circle(DISP, WorkZone_RightPoints[i], 1, cv::Scalar(0, 255, 0), -1);
			}
			cv::line(DISP, cv::Point(0, WorkZone_StraightLineFunction_Y(0, WorkZone_Line_Param_Right)),
					 cv::Point(Rize_Cols, WorkZone_StraightLineFunction_Y(Rize_Cols, WorkZone_Line_Param_Right)), cv::Scalar(0, 255, 255), 1, 4, 0);

			/* 中线 */
			cv::Point Point_LineStart, Point_LineEnd;
			Point_LineStart.x = (WorkZone_StraightLineFunction_X(0, WorkZone_Line_Param_Left) + WorkZone_StraightLineFunction_X(0, WorkZone_Line_Param_Right)) / 2;
			Point_LineStart.y = 0;
			Point_LineEnd.x = (WorkZone_StraightLineFunction_X(Rize_Rows, WorkZone_Line_Param_Left) + WorkZone_StraightLineFunction_X(Rize_Rows, WorkZone_Line_Param_Right)) / 2;
			Point_LineEnd.y = Rize_Rows;
			cv::line(DISP, Point_LineStart, Point_LineEnd, cv::Scalar(0, 255, 255), 1, 4, 0);

			/* 预瞄点标注 */
			// cv::Point PointFollow;
			// PointFollow.x = (WorkZone_BorderH - Line_Param[3]) / (Line_Param[1] / Line_Param[0]) + Line_Param[2];
			// PointFollow.y = WorkZone_BorderH;
			// cv::circle(image_rgb, PointFollow, 3, cv::Scalar(0, 255, 255), -1);

			break;
		}
		case WORKZONE_LEFT_OUT:
		{
			cv::putText(DISP, "WORKZONE_LEFT_OUT", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);

			cv::circle(DISP, cv::Point(WorkZone_Exit_Cone[1], WorkZone_Exit_Cone[0]), 1, cv::Scalar(0, 255, 0), -1);
			cv::circle(DISP, cv::Point(WorkZone_Exit_Point[1], WorkZone_Exit_Point[0]), 1, cv::Scalar(0, 0, 255), -1);

			// cv::line(img_res1_mat, cv::Point(0, Rize_Rows), cv::Point(WorkZone_Exit_Cone[1], WorkZone_Exit_Cone[0]), cv::Scalar(0, 255, 255), 1, 4, 0);

			break;
		}
		default:
		{
			break;
		}
		}
	}
	imshow("RGB_DISP", DISP);
	waitKey(1);
}