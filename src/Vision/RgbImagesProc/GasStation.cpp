/*********************************************************************************************************************
 * File name  : GasStation.cpp-加油站元素
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

#define Track_One  1 
#define Track_Two  2 

enum GasStation_type_e GasStation_Type = GASSTATION_NONE;

int GasStation_Enter_Cone_Left[2];
int GasStation_Enter_Cone_Right[2];
int GasStation_Exit_Cone[2];
int GasStation_Exit_Point[2];

//数组点集合
vector<cv::Point> GasStation_LeftPoints(CorpRize_Rows);
Vec4f GasStation_Line_Param_Left; //二维直线类型 有四个值 前两个值代表着直线的方向 后面两个值为直线上的一点
vector<cv::Point> GasStation_RightPoints(CorpRize_Rows);
Vec4f GasStation_Line_Param_Right;
vector<cv::Point> GasStation_MidPoints(CorpRize_Rows);
Vec4f GasStation_Line_Param_Mid;
int GasStation_BorderH;
int GasStation_RinningCnt = 0;
bool GasStation_IfFindExit = false;
bool GasStation_IfConeNone = false;
bool NumConfirm = false;
bool NumSignHave = false;
bool NumSignNone = false;
bool IfGetEnter = false;
bool IfFindExit = false;
bool IfExitClose = false;
int GasOutNum = 1;
int GasStation_Red_Sign_Position[2]; //记录红色坐标

int GasStation_INPoint[2];
bool GasStation_Find_INPoint = false;

int GasStation_OUTPoint[2];
bool GasStation_Find_OUTPoint = false;

bool GasStation_Chrek_OnTrack_Red(Mat &img0) //判断车模有没有驶过地标的思路，经历一个有先有后丢（下降沿感觉会合理一点）,得想想咋改
{

	int y, x;
	int sign_count = 0;
	for (y = img0.rows / 2; y < img0.rows ; y++) //从近到远扫距车一半像素点距离的区域
	{
		for (x = img0.cols * 0.1; x < img0.cols * 0.9; x++)
		{
			if (img0.at<uchar>(y, x) == White) //赛道上找到连续两点白点
			{
				return true;
			} //利用指针去访问Mat中的元素
		}
	}
	return false;
}

int GasStation_Chrek_OffTrack_Red(Mat &img1,cv::Rect rect) //检查赛道外的红色区域
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
	if (side_count_left > 0.1 * img1.cols)// 这个系数再调一下
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
    if (side_count_right > 0.1 * img1.cols)
    {
        return 2;
    }

    return 0;
}

float GasStation_StraightLineFunction_Y(int X, cv::Vec4f Line_Param)
{
	return ((Line_Param[1] / Line_Param[0]) * (X - Line_Param[2]) + Line_Param[3]);//直线公式
}

float GasStation_StraightLineFunction_X(int Y, cv::Vec4f Line_Param)
{
	return ((Y - Line_Param[3]) / (Line_Param[1] / Line_Param[0]) + Line_Param[2]);//直线公式
}
// int cone_ipts0_num, cone_ipts1_num;
// 0626有一次出泛型区的时候出去了，还不知道是啥原因，可能是误判了
// 0626 施工区的路径很差，小圆环会有点内切；
uint GasStation_GetNumber = 0;//这个记得清零，不然可能异常出加油站
void Check_GasStation()//判断加油站数字,感觉应该没问题
{
	if (GasStation_Type == GASSTATION_NONE && ROIs.size() == 2 && abs(ROIs[0].br().y - ROIs[1].br().y) > CorpRize_Rows * 0.3)
    {// 最后一个判断是为了防止地标被切割而误判
	
        int area_temp = 1;
        cv::Rect ROI_Near;
        for (size_t i = 0; i < ROIs.size(); i++)
        {
            if (ROIs[i].area() > area_temp)
            {
                area_temp = ROIs[i].area();
                ROI_Near = ROIs[i];
            }
        }
	
		if (ROI_Near.br().y > (CorpRize_Rows *0.50) && abs(((ROI_Near.tl().x + ROI_Near.br().x) / 2) - CorpRize_Cols*0.45) < CorpRize_Cols*0.20)
		{	
		//cout << "GasStation_Chrek_OffTrack_Red(red_bin_image, ROI_Near) = " << GasStation_Chrek_OffTrack_Red(red_bin_image, ROI_Near) << endl;
        switch (GasStation_Chrek_OffTrack_Red(red_bin_image, ROI_Near))
        {
            case 0:
            {
                GasStation_Type = GASSTATION_NONE;
                break;
            }
            case 1:
            {
                GasStation_Type = GASSTATION_LEFT_FOUND;
				//GasStation_RinningCnt++;
                break;
            }
            case 2:
            {
                GasStation_Type = GASSTATION_RIGHT_FOUND;
                break;
            }
            default:
            {
                break;
            }
        }
		}
	}
}
void Run_GasStation()
{
	// if(GasStation_RinningCnt == 1)
	// {
	// 	GasOutNum=1;
	// }
	// else
	// {
	// 	GasOutNum=2;
	// }
	
	 if (GasStation_Type == GASSTATION_LEFT_FOUND)// 找到红色地标状态（记得搜索地标的时候给W也限定一下范围）
    {
		// DeeplearningClass(Camera0,ROIs[0]);
		
        if (!GasStation_Chrek_OnTrack_Red(dilate_image))// 如果地标已经过去了就进入下一个状态
        {
            GasStation_Type = GASSTATION_LEFT_NEARDOOR;
            NumConfirm = false;
			//清空12计数
			TrackRedCla.ClearWriteOne();
			TrackRedCla.ClearWriteTwo();
        }
    }
    else if (GasStation_Type == GASSTATION_LEFT_NEARDOOR)
    {
		if(TrackRedCla.WriteOne_Num<4 &&TrackRedCla.WriteTwo_Num<4)
		{
			DeeplearningClass(Camera0,ROIs[0]);
		}	
		if((TrackRedCla.WriteOne_Num > TrackRedCla.WriteTwo_Num) &&TrackRedCla.WriteOne_Num>2)
		{
			GasOutNum = 1;
		}
		else if((TrackRedCla.WriteTwo_Num > TrackRedCla.WriteOne_Num) &&TrackRedCla.WriteTwo_Num>2)
		{
			GasOutNum = 2;
		}
        GasStation_SearchEnterCone_Left(dilate_image);// 右上角锥桶对施工区和加油站来说都挺极限的，看看加个偏差之类的，或者直接搜整个的右上角
		//cout << "GasOutNum =" << GasOutNum << endl;
        if (GasStation_Enter_Cone_Left[0] >= dilate_image.rows - 1)
        {
            GasStation_Type = GASSTATION_LEFT_IN;
            NumSignHave = false;
            NumSignNone = false;
            IfGetEnter = false;
        }
    }
    else if (GasStation_Type == GASSTATION_LEFT_IN)
    {
        GasStation_SearchINPoint_Left(dilate_image);
		//cout << "GasStation_SearchINPoint_Left ="  << endl;
        if (IfGetEnter)
        {
            GasStation_Type = GASSTATION_LEFT_RUNNING;
            NumSignHave = false;
            NumSignNone = false;
            IfFindExit = false;
        }
    }
    else if (GasStation_Type == GASSTATION_LEFT_RUNNING)
    {
        GasStation_SearchMidLine_Left(dilate_image);
        if (IfFindExit)
        {
            GasStation_Type = GASSTATION_LEFT_OUT;
            IfExitClose = false;
        }
    }
    else if (GasStation_Type == GASSTATION_LEFT_OUT)
    {
        GasStation_SearchOUTPoint_Left(dilate_image);
        if (GasStation_BackToRoad(image_src))
        {
            GasStation_Type = GASSTATION_NONE;
			TrackRedCla.ClearWriteOne();
			TrackRedCla.ClearWriteTwo();
        }   
    }
}

void GasStation_SearchINPoint_Left(Mat &img_bin) //检测左上锥桶函数
{
	int y, x;
	GasStation_INPoint[0] = -1;
	GasStation_INPoint[1] = -1;
	for (y = 5; y < img_bin.rows; y++) // h, w起始点可以考虑重设
	{
		for (x = 10; x < img_bin.cols; x++)
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y + 1, x) == White &&
				img_bin.at<uchar>(y + 2, x) && img_bin.at<uchar>(y + 3, x))
			{
				GasStation_INPoint[0] = y;
				if(GasOutNum ==1)
				{
					GasStation_INPoint[1] = x + 10; //动态偏移量的比例要调
				}
				else if(GasOutNum ==2)
				{
					GasStation_INPoint[1] = x + 10;
				}
				y=img_bin.cols;
				break;
			}
		}
	}

	int WhiteCount = 0;
    for (y = img_bin.rows * 3 / 4; y < img_bin.rows; y++)
    {
        for (x = img_bin.cols / 2 - 5; x < img_bin.cols; x++)
        {
            if (img_bin.at<uchar>(y, x) == White)
            {
                WhiteCount++;
            }
        }
    }
  	//cout << "WhiteCount =" << WhiteCount << endl;
    if (WhiteCount > 5)
    {
		//IfGetEnter = true;
        NumSignHave = true;
    }

    if (WhiteCount < 5 && NumSignHave)
    {
        NumSignNone = true;
    }

    if (WhiteCount > 5 && NumSignHave && NumSignNone)
    {
        IfGetEnter = true;
        NumSignHave = false;
        NumSignNone = false;
    }
	// cout << "WhiteCount =" << WhiteCount << endl;
	// cout << "NumSignHave =" << NumSignHave << endl;
	// cout << "NumSignNone =" << NumSignNone << endl;
}

void GasStation_SearchEnterCone_Left(Mat &img_bin) // 检测左入口处锥桶函数
{
	int Check_Count;	// 检查点计数
	int Point_Check[2]; // 检查点缓冲区

	/* 寻找入口左下锥桶(从右往左扫，扫完一行就从下到上) */
	Check_Count = 0;							// 初始化检查点计数值
	Point_Check[0] = -1;						// 初始化检查点缓冲区H坐标
	Point_Check[1] = -1;						// 初始化检查点缓冲区W坐标
	for (int y = img_bin.rows - 1; y >= 0; y--) // 从下往上扫描
	{
		for (int x = img_bin.cols; x >= 1; x--) // 从图像中线开始从右往左扫描
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y, x - 1) == White) // 如果同一行中连续两列都是白色，则认为可能是目标点
			{
				if (x <= Point_Check[1]) // 如果当前白色点的x比之前记录的x要小(或等于)，则开始进行检查
				{
					if (Check_Count == 0) // 如果是刚开始转折，则将前一个点记录为疑似目标点
					{
						GasStation_Enter_Cone_Left[0] = Point_Check[0]; // 记录当前白色点的H坐标
						GasStation_Enter_Cone_Left[1] = Point_Check[1]; // 记录当前白色点的W坐标
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
	int Star_x=0;
	if (GasStation_Type == GASSTATION_LEFT_NEARDOOR &&ROIs.size() == 1)
	{
		Star_x = ROIs[0].tl().x - 3;
	}
	else
	{
		Star_x = img_bin.cols * 3 / 4;
	}
	for (int x = Star_x; x >= 0; x--) // 从图像中线开始从右往左扫描
	{
		for (int y = img_bin.rows; y >= 1; y--) // 从下往上扫描
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y - 1, x) == White) // 如果同一列中连续两行都是白色，则认为可能是目标点
			{
				if (y <= Point_Check[0]) // 如果当前白色点的H比之前记录的H要小(或等于)，则开始进行检查
				{
					if (Check_Count == 0) // 如果是刚开始转折，则将前一个点记录为疑似目标点
					{
						GasStation_Enter_Cone_Right[0] = Point_Check[0]; // 记录当前白色点的H坐标
						GasStation_Enter_Cone_Right[1] = Point_Check[1]; // 记录当前白色点的W坐标
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
void GasStation_SearchMidLine_Left(Mat &img_bin)//锥桶区内巡线
{
	int UpPoint[2];
	int DownPoint[2];
	int y, x, x_left, x_right;

	/* 寻找施工区最上点 */
	for ( y = 0; y < img_bin.rows - 1; y++)
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
		for ( y = img_bin.rows - 1; y >= 1; y--)
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
	GasStation_BorderH = (DownPoint[0] + UpPoint[0]) / 2;
	
	/* 边界H加第三象限找左边线拟合 */
	GasStation_LeftPoints.clear();
	int GasStation_TouchBorderH = 0;
	bool get_start_left = false;
	int left_x_last = 0;
	int left_y_last = img_bin.rows;
	for (x_left = 0; x_left < img_bin.cols / 2; x_left++)
	{
		for (y = img_bin.rows - 1; y > GasStation_BorderH; y--)
		{
			if (img_bin.at<uchar>(y, x_left) == White && img_bin.at<uchar>(y - 1, x_left) == White)
			{
				if(y - left_y_last >  3 && GasStation_LeftPoints.size() > 2)
				{
					continue;
				}
				GasStation_LeftPoints.push_back(cv::Point(x_left, y));
				left_x_last =x_left;
				left_y_last =y;
				break;
			}	
		}
		if(y == GasStation_BorderH)
		{
                break;
		}	
	}
	 if (GasStation_LeftPoints.size() == 0)
    {
        GasStation_LeftPoints.push_back(cv::Point(0, img_bin.rows / 2));
        GasStation_LeftPoints.push_back(cv::Point(img_bin.cols / 2 , 0));
    }
	cv::fitLine(GasStation_LeftPoints, GasStation_Line_Param_Left, cv::DIST_L2, 0, 1e-2, 1e-2);
	
	/* 第四象限加第一象限一个点拟合右边线 */
	GasStation_RightPoints.clear();
	bool right_x_last = img_bin.cols / 2;
	bool right_y_last = img_bin.rows;
	for (int y = img_bin.rows ; y > img_bin.rows/2; y--)
	{
		for (x_right = img_bin.cols / 2; x_right < img_bin.cols - 1; x_right++)
		{
			if (img_bin.at<uchar>(y, x_right) == White && img_bin.at<uchar>(y, x_right + 1) == White)
			{
				if(y - right_y_last > 3 &&GasStation_RightPoints.size()>2)
				{
					continue;
				}
				GasStation_RightPoints.push_back(cv::Point(x_right, y));
				right_x_last = x_right;
				right_y_last = y;
				break;
			}
		}
	}
	
	if (GasStation_RightPoints.size() == 0)
	{
		GasStation_RightPoints.push_back(cv::Point(img_bin.cols - 1, img_bin.rows - 1));
	}
	// cout << "GO THIS =" << endl;
	for (x_right = img_bin.cols * 3 / 4; x_right > img_bin.cols / 2; x_right--)
	{
		for (int y = GasStation_BorderH; y > 0; y--)
		{
			if (img_bin.at<uchar>(y, x_right) == White && img_bin.at<uchar>(y - 1, x_right) == White)
			{
				GasStation_RightPoints.push_back(cv::Point(x_right, y));
				//cout << "GO THIS =" << endl;
				x_right = img_bin.cols / 2;
				break;
			}
		}
	}
	// cout << "GO THIS =" << endl;
	cv::fitLine(GasStation_RightPoints, GasStation_Line_Param_Right, cv::DIST_L2, 0, 1e-2, 1e-2);
    
	/* 判断出口 */ //看第四象限上半部分是不是很少白点了(或者看最上白点H坐标)
	int WhiteCount = 0;
	for (int y = img_bin.rows /2; y < img_bin.rows * 3 / 4; y++)
	{
		for (int x = img_bin.cols * 5 / 8 ; x < img_bin.cols; x++)
		{
			if (img_bin.at<uchar>(y, x) == White)
			{
				WhiteCount++;
			}
		}
	}
	if (GasOutNum == 1)
    {
        if (WhiteCount < 5)
        {
            IfFindExit = true;
        }
    }
    else if (GasOutNum == 2)
    {
        if (WhiteCount < 3)
        {
            NumSignNone = true;
        }
        if (WhiteCount > 5 && NumSignNone)
        {
            NumSignHave = true;
        }
        if (WhiteCount < 5 && NumSignNone && NumSignHave)
        {
            IfFindExit = true;
            NumSignHave = false;
            NumSignNone = false;
        }
        }
}
void GasStation_SearchOUTPoint_Left(Mat &img_bin)
{
	/* 寻找出口处锥桶 */ // 因为会变化所以不能只在第一象限(先在第一象限下半部分找，找不到了就扫第三象限)
	GasStation_Exit_Cone[0] = -1;
	GasStation_Exit_Cone[1] = -1;
	for (int x = img_bin.cols - 1; x >= img_bin.cols / 2; x--) // 从图像中线开始从右往左扫描
	{
		for (int y = img_bin.rows / 2; y >= img_bin.rows / 4; y--) // 从下往上扫描
		{
			if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y - 1, x) == White)
			{
				GasStation_Exit_Cone[0] = y;
				GasStation_Exit_Cone[1] = x;
				x = -1;
				break;
			}
		}
	}

	if (GasStation_Exit_Cone[0] == -1 && GasStation_Exit_Cone[1] == -1)
	{
		for (int x = img_bin.cols / 2; x >= 0; x--) // 从图像中线开始从右往左扫描
		{
			for (int y = img_bin.rows - 1; y >= img_bin.rows / 2; y--) // 从下往上扫描
			{
				if (img_bin.at<uchar>(y, x) == White && img_bin.at<uchar>(y - 1, x) == White)
				{
					GasStation_Exit_Cone[0] = y;
					GasStation_Exit_Cone[1] = x;
					x = -1;
					break;
				}
			}
		}
	}

	/* 对原图进行二值化 */ // 看看要不要加个啥if GasStation_Exit_Cone[0] > 0之类的
	// 这里记得还是开个图像看看
	cv::Mat img_gra_temp;
	cv::Mat img_bin_temp;
	//这个地方可以更改一手
	cv::cvtColor(image_rgb, img_gra_temp, cv::COLOR_BGR2GRAY); // 如果还有其他地方要用的话就把这几句放到公共位置让他们只运行一次
	cv::threshold(img_gra_temp, img_bin_temp, 255, 255, cv::THRESH_OTSU);
	int x;
	bool IfFindWhite = false;
	for (x = GasStation_Exit_Cone[1]; x < img_bin.cols - 1; x++) // 从图像中线开始从右往左扫描
	{
		if (img_bin_temp.at<uchar>(GasStation_Exit_Cone[0], x) == White && img_bin_temp.at<uchar>(GasStation_Exit_Cone[0], x + 1) == White)
		{ // 还是要注意一下踩着白点还是黑点
			IfFindWhite = true;
		}
		if (IfFindWhite && img_bin_temp.at<uchar>(GasStation_Exit_Cone[0], x) == Black && img_bin_temp.at<uchar>(GasStation_Exit_Cone[0], x + 1) == Black)
		{
			break;
		}
	}
	GasStation_Exit_Point[0] = GasStation_Exit_Cone[0];
	if(GasOutNum == 1)
	{
		GasStation_Exit_Point[1] = GasStation_Exit_Cone[1] + 8;
	}
	else if(GasOutNum == 2)
	{
		GasStation_Exit_Point[1] = GasStation_Exit_Cone[1] + 18;
	}
	

	if (GasStation_Exit_Point[0] > img_bin.cols * 3 / 4)
    {
        IfExitClose = true;
    }
	// if (img_bin.rows / 2 <= GasStation_Exit_Cone[0] && GasStation_Exit_Cone[0] < img_bin.rows &&
	// 	0 <= GasStation_Exit_Cone[1] && GasStation_Exit_Cone[1] < img_bin.cols / 2)
	// {
	// 	int WhiteCount = 0;
	// 	for (int y = img_bin.rows / 4; y < img_bin.rows / 2; y++) // 第四象限下部分
	// 	{
	// 		for (int x = 0; x < img_bin.cols / 2; x++)
	// 		{
	// 			if (img_bin.at<uchar>(y, x) == White)
	// 			{
	// 				WhiteCount++;
	// 			}
	// 		}
	// 	}
	// 	if (WhiteCount < 5)
	// 	{
	// 		GasStation_IfConeNone = true;
	// 	}
	// }
}
bool GasStation_BackToRoad(Mat &img_raw) //感觉用灰头判有没有回到路会好一点
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

bool GasStation_Track_HaveRedSign = false;
// void GasStation_Proc(Mat &RGBCamera0)
// {
// 	Rgb2Resize_proc(RGBCamera0, image_rgb, CorpRize_Cols, CorpRize_Rows); //图像压缩
// 	Rgb2LabBin_proc(image_rgb, red_bin_image);								  // HSV提取红色，后续研究看需不需要腐蚀膨胀
// 	GasStation_Track_HaveRedSign = GasStation_Chrek_OnTrack_Red(red_bin_image);
// 	if (GasStation_Type == GASSTATION_NONE && GasStation_Track_HaveRedSign && GasStation_Red_Sign_Position[0] > red_bin_image.rows * 0.5)
// 	{
// 		Check_GasStation();
// 	}
// 	if (GasStation_Type != GASSTATION_NONE)
// 	{
// 		Run_GasStation();
// 	}
// 	GasStation_Control();
// }
void GasStation_Disp(Mat &DISP)
{
	// circle(DISP, Point(GasStation_INPoint[1], GasStation_INPoint[0]), 0.1, Scalar(255, 255, 255), -1);

	if (GasStation_Type != GASSTATION_NONE)
	{
		switch (GasStation_Type)
		{
		// case GASSTATION_LEFT_BEGIN:
		// {
		// 	cv::putText(DISP, "GASSTATION_LEFT_BEGIN", cv::Point(0, Rize_Rows),
		// 				cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);
		// 	break;
		// }
		case GASSTATION_LEFT_NEARDOOR:
		{
			cv::putText(DISP, "GASSTATION_LEFT_NEARDOOR", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);
			int Enter_Cone_Mid[2] = {(GasStation_Enter_Cone_Left[0] + GasStation_Enter_Cone_Right[0]) / 2, (GasStation_Enter_Cone_Left[1] + GasStation_Enter_Cone_Right[1]) / 2};
			cv::circle(DISP, cv::Point(GasStation_Enter_Cone_Left[1], GasStation_Enter_Cone_Left[0]), 2, cv::Scalar(0, 255, 0), -1);
			cv::circle(DISP, cv::Point(GasStation_Enter_Cone_Right[1], GasStation_Enter_Cone_Right[0]), 2, cv::Scalar(0, 255, 0), -1);
			cv::circle(DISP, cv::Point(Enter_Cone_Mid[1], Enter_Cone_Mid[0]), 2, cv::Scalar(0, 255, 0), -1);
			break;
		}
		case GASSTATION_LEFT_IN:
		{
			cv::putText(DISP, "GASSTATION_LEFT_IN", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);
			cv::circle(DISP, cv::Point(GasStation_INPoint[1], GasStation_INPoint[0]), 2, cv::Scalar(0, 255, 0), -1);
			break;
		}
		case GASSTATION_LEFT_RUNNING:
		{
			cv::putText(DISP, "GASSTATION_LEFT_RUNNING", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);

			/* 寻中线上边界 */
			cv::line(DISP, cv::Point(0, GasStation_BorderH), cv::Point(Rize_Cols, GasStation_BorderH), cv::Scalar(255, 0, 255), 1);

			/* 左边界散点和拟合直线 */
			for (int i = 0; i < GasStation_LeftPoints.size(); i++)
			{
				cv::circle(DISP, GasStation_LeftPoints[i], 1, cv::Scalar(0, 255, 0), -1);
			}
			cv::line(DISP, cv::Point(0, GasStation_StraightLineFunction_Y(0, GasStation_Line_Param_Left)),
					 cv::Point(Rize_Cols, GasStation_StraightLineFunction_Y(Rize_Cols, GasStation_Line_Param_Left)), cv::Scalar(0, 255, 255), 1, 4, 0);

			/* 右边界散点和拟合直线 */
			for (int i = 0; i < GasStation_RightPoints.size(); i++)
			{
				cv::circle(DISP, GasStation_RightPoints[i], 1, cv::Scalar(0, 255, 0), -1);
			}
			cv::line(DISP, cv::Point(0, GasStation_StraightLineFunction_Y(0, GasStation_Line_Param_Right)),
					 cv::Point(Rize_Cols, GasStation_StraightLineFunction_Y(Rize_Cols, GasStation_Line_Param_Right)), cv::Scalar(0, 255, 255), 1, 4, 0);

			/* 中线 */
			cv::Point Point_LineStart, Point_LineEnd;
			Point_LineStart.x = (GasStation_StraightLineFunction_X(0, GasStation_Line_Param_Left) + GasStation_StraightLineFunction_X(0, GasStation_Line_Param_Right)) / 2;
			Point_LineStart.y = 0;
			Point_LineEnd.x = (GasStation_StraightLineFunction_X(Rize_Rows, GasStation_Line_Param_Left) + GasStation_StraightLineFunction_X(Rize_Rows, GasStation_Line_Param_Right)) / 2;
			Point_LineEnd.y = Rize_Rows;
			cv::line(DISP, Point_LineStart, Point_LineEnd, cv::Scalar(0, 255, 255), 1, 4, 0);

			/* 预瞄点标注 */
			// cv::Point PointFollow;
			// PointFollow.x = (GasStation_BorderH - Line_Param[3]) / (Line_Param[1] / Line_Param[0]) + Line_Param[2];
			// PointFollow.y = GasStation_BorderH;
			// cv::circle(image_rgb, PointFollow, 3, cv::Scalar(0, 255, 255), -1);
			break;
		}
		case GASSTATION_LEFT_OUT:
		{
			cv::putText(DISP, "GASSTATION_LEFT_OUT", cv::Point(0, Rize_Rows),
						cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(0, 0, 255), 1.2);

			cv::circle(DISP, cv::Point(GasStation_Exit_Cone[1], GasStation_Exit_Cone[0]), 1, cv::Scalar(0, 255, 0), -1);
			cv::circle(DISP, cv::Point(GasStation_Exit_Point[1], GasStation_Exit_Point[0]), 1, cv::Scalar(0, 0, 255), -1);

			// cv::line(img_res1_mat, cv::Point(0, Rize_Rows), cv::Point(GasStation_Exit_Cone[1], GasStation_Exit_Cone[0]), cv::Scalar(0, 255, 255), 1, 4, 0);

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