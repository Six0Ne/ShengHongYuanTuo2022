/*********************************************************************************************************************
 * File name  : RgbImageProc.cpp-彩色摄像头预处理
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
#define White 255
#define Black 0

std::vector<cv::Rect> ROIs;// 注意一下ROI是针对原图的
// std::vector<MODELRESULT> ModelResult;

int rgb_servo = 0, rgb_motor = 70;
Mat  image_rgb, hsv_image, lab_image, red_bin_image, erode_image, dilate_image, roi_image, resize_garyimage;

Mat ero_elem = getStructuringElement(MORPH_RECT, Size(3, 3));
Mat dil_elem = getStructuringElement(MORPH_RECT, Size(3, 3));

vector<vector<Point>> contours_image;
Scalar RED_2LABLOW = Scalar(25, 148, 108);
Scalar RED_2LABHIGH = Scalar(180, 208, 183);


Scalar RED_2HSVLOW0 = Scalar(0, 120, 70);
Scalar RED_2HSVHIGH0 = Scalar(8, 255, 255);
Scalar RED_2HSVLOW1 = Scalar(242, 120, 70);
Scalar RED_2HSVHIGH1 = Scalar(255, 255, 255);


void Rgb2Resize_proc(Mat& image0, Mat& image1, int image_x, int image_y)
{
	resize(image0, image1, Size(image_x, image_y));//压缩图片
}

void Rgb2Gray(Mat& image0, Mat& image1)
{
	cvtColor(image0, image1, COLOR_BGR2GRAY);//0.3ms//压缩图片
}

void Rgb2LabBin_proc(Mat& image0, Mat& image1)
{
	Mat lab_image;
	cvtColor(image0, lab_image, COLOR_BGR2Lab);
	inRange(lab_image, RED_2LABLOW, RED_2LABHIGH, image1);
}

void Rgb2HsvBin_proc(Mat& image0, Mat& image1)
{
	Mat hsvtemp1_image, hsvtemp2_image;
	cvtColor(image_rgb, hsv_image, COLOR_BGR2HSV);
	inRange(hsv_image, RED_2HSVLOW0, RED_2HSVHIGH0, hsvtemp1_image);
	inRange(hsv_image, RED_2HSVLOW1, RED_2HSVHIGH1, hsvtemp2_image);
	image1 = hsvtemp1_image + hsvtemp2_image;
}

void Bin2Erode_proc(Mat& image0, Mat& image1, Mat& element)
{
	erode(image0, image1, element);
}

void Bin2Dilate_proc(Mat& image0, Mat& image1, Mat& element)
{
	dilate(image0, image1, element);
}
void FilterContours(Mat& Img, vector<cv::Rect>& Rects)//四点法定位地标
{
	vector<vector<Point> > red_contours;
	findContours(Img.clone(), red_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//vector<RotatedRect> red_box(red_contours.size()); //定义最小外接矩形集合
	//ROI = Mat::zeros(64, 64, CV_32F);//每次找地标的时候都初始化为黑的
	Rects.clear();
	for (size_t i = 0; i < red_contours.size(); i++)
	{
		Rect red_box = boundingRect(Mat(red_contours[i]));  //计算每个轮廓最小外接矩形
		bool in_track = (resize_garyimage.at<uchar>(red_box.tl().y, red_box.tl().x) > thres) && (resize_garyimage.at<uchar>(red_box.br().y, red_box.br().x) > thres) &&
			(resize_garyimage.at<uchar>(red_box.tl().y + red_box.height, red_box.tl().x) > thres) && (resize_garyimage.at<uchar>(red_box.br().y - red_box.height, red_box.br().x) > thres);
		if (in_track&&red_box.area()>1)//裁剪出来的红色区域四点在赛道上
		{
			// red_box = red_box + Point(int(red_box.tl().x * ratio_X - red_box.tl().x), (red_box.tl().y * ratio_Y - red_box.tl().y));//平移到原图
			// red_box = red_box + Size(int(red_box.width * ratio_X - red_box.width), (red_box.height * ratio_Y - red_box.height));
			//ROI = Camera0(Rect(red_box.tl().x, red_box.tl().y, red_box.width, red_box.height)); //把地标裁出来
			//DeeplearningClass(ROI);
 			Rects.push_back(red_box);//输出筛选过后的矩形框信息
			//rectangle(image_src, red_box, Scalar(255), 1);
		}
		//cout << "red_contours.size() =" << red_contours.size() << endl;
		//rectangle(image_src, red_box, Scalar(255), 1);
	}
	
	// imshow("image_rgb", image_src);

	// drawContours(image_rgb, contours, -1, Scalar(0, 0, 255), 1); //> 在RGB图像画出轮廓 
}
void RgbImgProc(Mat& img0)
{
	Rgb2Resize_proc(img0, image_rgb, CorpRize_Cols, CorpRize_Rows);
	Rgb2LabBin_proc(image_rgb, red_bin_image);
	Bin2Dilate_proc(red_bin_image, dilate_image, ero_elem);
}
void Target_Cropping(Mat& img0)
{
	Rgb2Gray(image_rgb, resize_garyimage);
	FilterContours(img0, ROIs);
    //cout << "ROIs =" << ROIs.size() << endl;
}

// void  RgbImgProc(void)
// {
// 	Target_Cropping(Camera0);
// 	if(Yroad_Type == YROAD_NONE)
// 		Chrek_Ramp();
// 	if(Ramp_Type!=RAMP_NONE)
// 	{
// 		rgb_motor = 60;
// 		Run_Ramp();
// 	}	
// }

bool Track_HaveRedSign=false;
void WorkZone_TASK(Mat& RGBCamera0)
{
	Rgb2Resize_proc(RGBCamera0, image_rgb, Rize_Cols, Rize_Rows);//图像压缩//图像压缩
	Rgb2LabBin_proc(image_rgb,red_bin_image);//HSV提取红色，后续研究看需不需要腐蚀膨胀
	Track_HaveRedSign = WorkZone_Chrek_OnTrack_Red(red_bin_image);
	if (WorkZone_Type == WORKZONE_NONE&&Track_HaveRedSign&&WorkZone_Red_Sign_Position[0] > red_bin_image.rows * 0.5)
	{
		Check_WorkZone();
	}
	if (WorkZone_Type != WORKZONE_NONE) 
	{
		Run_WorkZone();
	}
   		WorkZone_Control();
		WorkZone_Disp(image_rgb);

}
bool Chrek_WorkZoneOrGasStation(Mat &img0) //在图像范围内一半寻找赛道上的标志
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
void WorkZoneOrGasStation_TASKJUDGE()
{
	/* 地标定位 */
    if (GasStation_Type != GASSTATION_LEFT_IN && GasStation_Type != GASSTATION_LEFT_RUNNING && GasStation_Type != GASSTATION_LEFT_OUT &&
        WorkZone_Type != WORKZONE_LEFT_IN && WorkZone_Type != WORKZONE_LEFT_RUNNING && WorkZone_Type != WORKZONE_LEFT_OUT)
    {
        Target_Cropping(dilate_image);// 加上根据状态开启
    }
	if (Garage_Type == GARAGE_NONE && 
	Cross_Type == CROSS_NONE && Yroad_Type == YROAD_NONE)
    {
        Check_GasStation();
        if (GasStation_Type == GASSTATION_NONE)
        {
            Check_WorkZone();
        }
    }

    if (Garage_Type != GARAGE_NONE)
    {
        GasStation_Type = GASSTATION_NONE;
        WorkZone_Type = WORKZONE_NONE;
    }
    else if (GasStation_Type != GASSTATION_NONE)
    {
        WorkZone_Type = WORKZONE_NONE;
    }
}

void WorkZoneOrGasStation_TASKPROC()
{
	if (GasStation_Type != GASSTATION_NONE)
    {
        Run_GasStation();
    }
    else if (WorkZone_Type != WORKZONE_NONE)
    {
        Run_WorkZone();
    }
}

void WorkZoneOrGasStation_TASKCONTROL()
{
	float error_rgb = 0;
	rgb_motor=70;
	RGBPID_Servo.P = 0.55;
	if ((GasStation_Type == GASSTATION_LEFT_NEARDOOR))
	{
		RGBPID_Servo.P = 0.55;
		rgb_motor=60;
		error_rgb = CorpRize_Cols / 2  - (GasStation_Enter_Cone_Left[1] + GasStation_Enter_Cone_Right[1]) / 2;
	}
	else if (GasStation_Type == GASSTATION_LEFT_IN)
	{
		RGBPID_Servo.P = 0.65;
		if(GasOutNum == 1)
		{
			rgb_motor=51;
		}
		else if(GasOutNum == 2)
		{
			rgb_motor=51;
		}
		error_rgb = CorpRize_Cols / 2 - GasStation_INPoint[1];
		// cout << "servo = " << servo << endl;
	}
	else if (GasStation_Type == GASSTATION_LEFT_RUNNING)
	{
		RGBPID_Servo.P = 0.55;
		if(GasOutNum == 1)
		{
			RGBPID_Servo.P = 0.55;
			rgb_motor=51;
			error_rgb = (CorpRize_Cols / 2 - 5) - (GasStation_StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Left) +
									 0.7 * (GasStation_StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Right) - GasStation_StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Left)));
		}
		else if(GasOutNum == 2)	
		{
			RGBPID_Servo.P = 0.55;
			rgb_motor=55;
			error_rgb = (CorpRize_Cols / 2) - (GasStation_StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Left) +
									 0.70 * (GasStation_StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Right) - GasStation_StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Left)));
		}						 
		// cout << "servo = " << (GasStation_StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Left) +
		//                   0.7 * (GasStation_StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Right) - StraightLineFunction_X(0.3 * Rize_Rows, GasStation_Line_Param_Left))) << endl;
	}
	else if (GasStation_Type == GASSTATION_LEFT_OUT)
	{
		RGBPID_Servo.P = 0.53;
		rgb_motor=60;
		error_rgb = CorpRize_Cols / 2 - GasStation_Exit_Point[1];
	}
	else if ((WorkZone_Type == WORKZONE_LEFT_NEARDOOR))
	{
		RGBPID_Servo.P = 0.75;
		rgb_motor=80;
		error_rgb = CorpRize_Cols / 2 - (WorkZone_Enter_Cone_Left[1] + WorkZone_Enter_Cone_Right[1]) / 2;
	}
	else if (WorkZone_Type == WORKZONE_LEFT_IN)
	{
		RGBPID_Servo.P = 0.75;
		rgb_motor=70;
		error_rgb = CorpRize_Cols / 2 - WorkZone_INPoint[1];
		// cout << "servo = " << servo << endl;
	}
	else if (WorkZone_Type == WORKZONE_LEFT_RUNNING)
	{
		RGBPID_Servo.P = 0.75;
		rgb_motor=70;
		error_rgb = (CorpRize_Cols / 2 ) - (WorkZone_StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Left) +
									 0.7 * (WorkZone_StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Right) - WorkZone_StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Left)));
		// cout << "servo = " << (WorkZone_StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Left) +
		//                   0.7 * (WorkZone_StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Right) - StraightLineFunction_X(0.3 * Rize_Rows, WorkZone_Line_Param_Left))) << endl;
	}
	else if (WorkZone_Type == WORKZONE_LEFT_OUT)
	{
		RGBPID_Servo.P = 0.65;
		rgb_motor=70;
		error_rgb = CorpRize_Cols / 2 - WorkZone_Exit_Point[1];
	}
	float RGB_angle = Servo_PID(&RGBPID_Servo, error_rgb);
	// float RGB_angle =0.3*error_rgb;
	// cout << "RGB_angle = " << RGB_angle << endl;
	rgb_servo = RGB_angle * 100;
}
