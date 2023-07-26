#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
using namespace cv;
using namespace std;
#define NameWindow  "图片处理效果"
//显示矩阵
Mat  Disp_gray, Disp_bin, Disp_edge;
//窗口显示多张图像矩阵

void imshowMany(const string& _winName, const vector<Mat>& ployImages)
{
	int nImg = (int)ployImages.size();//获取在同一画布中显示多图的数目  


	Mat dispImg;


	int size;
	int x, y;
	//若要在OpenCV实现同一窗口显示多幅图片，图片要按矩阵方式排列，类似于Matlab中subplot();     
	//多图按矩阵排列的行数  ，h: 多图按矩阵排列的的数    
	int w, h;


	float scale;//缩放比例  
	int max;


	if (nImg <= 0)
	{
		printf("Number of arguments too small....\n");
		return;
	}
	else if (nImg > 12)
	{
		printf("Number of arguments too large....\n");
		return;
	}


	else if (nImg == 1)
	{
		w = h = 1;
		size = 400;
	}
	else if (nImg == 2)
	{
		w = 2; h = 1;//2x1  
		size = 400;
	}
	else if (nImg == 3 || nImg == 4)
	{
		w = 2; h = 2;//2x2  
		size = 400;
	}
	else if (nImg == 5 || nImg == 6)
	{
		w = 3; h = 2;//3x2  
		size = 300;
	}
	else if (nImg == 7 || nImg == 8)
	{
		w = 4; h = 2;//4x2  
		size = 300;
	}
	else
	{
		w = 4; h = 3;//4x3  
		size = 200;
	}


	dispImg.create(Size(100 + size * w, 30 + size * h), CV_8UC3);//根据图片矩阵w*h，创建画布，可线的图片数量为w*h  


	for (int i = 0, m = 20, n = 20; i < nImg; i++, m += (20 + size))
	{
		x = ployImages[i].cols; //第(i+1)张子图像的宽度(列数)  
		y = ployImages[i].rows;//第(i+1)张子图像的高度（行数）  


		max = (x > y) ? x : y;//比较每张图片的行数和列数，取大值  
		scale = (float)((float)max / size);//计算缩放比例  


		if (i % w == 0 && m != 20)
		{
			m = 20;
			n += 20 + size;
		}


		Mat imgROI = dispImg(Rect(m, n, (int)(x / scale), (int)(y / scale))); //在画布dispImage中划分ROI区域  
		resize(ployImages[i], imgROI, Size((int)(x / scale), (int)(y / scale))); //将要显示的图像设置为ROI区域大小  
	}
	namedWindow(_winName);
	imshow(_winName, dispImg);
}
//视觉显示
// void vision_display_init()
// {

// }
void vision_display(Mat& image_disp, Mat& imgae_mask)
{
	// // //爬边起始点可视化
	//line(Disp_gray, Point(0, begin_y), Point(Disp_gray.size().width/2-begin_x, begin_y), Scalar(0, 0, 0));
	//line(Disp_gray, Point(Disp_gray.size().width/2+begin_x, begin_y), Point(Disp_gray.size().width, begin_y), Scalar(0, 0, 0));

	// circle(Disp_gray, Point(Disp_gray.size().width/2-begin_x, begin_y), 2, Scalar(0, 0, 255), -1);
	// circle(Disp_gray, Point(Disp_gray.size().width/2+begin_x, begin_y), 2, Scalar(0, 0, 255), -1);


	circle(imgae_mask, Point(212, 120), 300, Scalar(0, 0, 0), -1);
	// circle(imgae_mask, Point(212, 120), 212, Scalar(0, 0, 0), -1);
	// circle(imgae_mask, Point(212, 120), 212, Scalar(0, 0, 0), -1);

	//爬边起始点可视化

	for (unsigned char i = 0; i < rpts0s_num; i++)
	{
		circle(imgae_mask, Point(rpts0s[i][0], rpts0s[i][1]), 0.05, Scalar(255, 255, 255), 1);
	}
	for (unsigned char i = 0; i < rpts1s_num; i++)
	{
		circle(imgae_mask, Point(rpts1s[i][0], rpts1s[i][1]), 0.05, Scalar(255, 255, 255), 1);
	}
	for (unsigned char i = 0; i < rptsn_num; i++)
	{
		circle(imgae_mask, Point(rptsn[i][0], rptsn[i][1]), 0.05, Scalar(255, 255, 255), 1);
		// Point RC(rptsc1[i][0], rptsc1[i][1]);
		// circle(imgae_mask, RC, 0.1, Scalar(255, 255, 255), -1);	 
	}
	if (Yroad_Type == YROAD_NEAR)
	{
		for (unsigned char i = 0; i < faryroad_rpts1s_num; i++)
		{
			circle(imgae_mask, Point(faryroad_rpts1s[i][0], faryroad_rpts1s[i][1]), 0.1, Scalar(255, 255, 255), -1);
			// Point RC(rptsc1[i][0], rptsc1[i][1]);
			// circle(imgae_mask, RC, 0.1, Scalar(255, 255, 255), -1);	 
		}
	}
	if (Yroad_Type == GENERIC_IN)
	{
		for (unsigned char i = 0; i < generic_rpts1s_num; i++)
		{
			Point RF(generic_rpts1s[i][0], generic_rpts1s[i][1]);
			circle(imgae_mask, RF, 0.1, Scalar(255, 255, 255), -1);
		}
	}
	if (Yroad_Type == GENERIC_OUT)
	{
		for (unsigned char i = 0; i < generic_rpts0s_num; i++)
		{
			Point GOUT(generic_rpts0s[i][0], generic_rpts0s[i][1]);
			circle(imgae_mask, GOUT, 0.1, Scalar(255, 255, 255), -1);
		}
	}
	circle(imgae_mask, Point(generic_rpts1s[generic_Ypt1_rpts1s_id][0], generic_rpts1s[generic_Ypt1_rpts1s_id][1]), 2, Scalar(0, 255, 0), -1);
	if (Garage_Type != GARAGE_NONE)//如果不是非车库
	{
		//打印目前元素信息
		cout << "Garage_Type =" << Garage_Type << endl;
		switch (Garage_Type)
		{
		case 1:
			printf("GARAGE_BEGINOUT_LEFT\n");
			break;
		case 2:
			printf("GARAGE_BEGINOUT_RIGHT\n");
			break;
		case 3:
			printf("GARAGE_OUT_LEFT\n");
			break;
		case 4:
			printf("GARAGE_OUT_RIGHT\n");
			break;
		case 5:
			printf("GARAGE_FOUND_LEFT\n");
			break;
		case 6:
			printf("GARAGE_FOUND_RIGHT\n");
			break;
		case 7:
			printf("GARAGE_IN_LEFT\n");
			break;
		case 8:
			printf("GARAGE_IN_RIGHT\n");
			break;
		case 9:
			printf("GARAGE_PASS_LEFT\n");
			break;
		case 10:
			printf("GARAGE_PASS_RIGHT\n");
			break;
		case 11:
			printf("GARAGE_STOP\n");
			break;
		default:
			break;

		}

	}
	if (Cross_Type != CROSS_NONE)//如果不是非十字路
	{

		//远线绘制
		for (unsigned char i = far_Lpt0_rpts0s_id; i < Images_Rows; i++)
		{
			Point LF(far_rpts0s[i][0], far_rpts0s[i][1]);
			circle(imgae_mask, LF, 0.1, Scalar(255, 255, 255), -1);
		}
		for (unsigned char i = far_Lpt1_rpts1s_id; i < Images_Rows; i++)
		{
			Point RF(far_rpts1s[i][0], far_rpts1s[i][1]);
			circle(imgae_mask, RF, 0.1, Scalar(255, 255, 255), -1);
		}
		// 远线L角点绘制
		Point FLL(far_rpts0s[far_Lpt0_rpts0s_id][0], far_rpts0s[far_Lpt0_rpts0s_id][1]);
		circle(imgae_mask, FLL, 2, Scalar(71, 97, 255), -1);
		Point FRL(far_rpts1s[far_Lpt1_rpts1s_id][0], far_rpts1s[far_Lpt1_rpts1s_id][1]);
		circle(imgae_mask, FRL, 2, Scalar(71, 97, 255), -1);
		//打印目前元素信息
		cout << "Cross_Type =" << Cross_Type << endl;
		switch (Cross_Type)
		{
		case 1:
			printf("CROSS_BEGIN\n");
			break;
		case 2:
			printf("CROSS_IN\n");
			break;
		default:
			break;

		}
	}
	if (Yroad_Type != YROAD_NONE)//如果不是非三叉路
	{

		//打印目前元素信息
		cout << "Yroad_Type =" << Yroad_Type << endl;
		switch (Yroad_Type)
		{
		case 1:
			printf("YROAD_FOUND\n");
			break;
		case 2:
			printf("YROAD_NEAR\n");
			break;
		case 3:
			printf("GENERIC_IN\n");
			break;
		case 4:
			printf("GENERIC_OUT\n");
			break;
		case 5:
			printf("YROAD_LEFT_OUT\n");
			break;
		case 6:
			printf("YROAD_RIGHT_OUT\n");
			break;
		default:
			break;

		}

	}
	if (Circle_Type != CIRCLE_NONE)
	{
		//打印目前元素信息
		cout << "circle_type =" << Circle_Type << endl;
		switch (Circle_Type)
		{
		case 1:
			printf("CIRCLE_LEFT_BEGIN\n");
			break;
		case 2:
			printf("CIRCLE_RIGHT_BEGIN\n");
			break;
		case 3:
			printf("CIRCLE_LEFT_IN\n");
			break;
		case 4:
			printf("CIRCLE_RIGHT_IN\n");
			break;
		case 5:
			printf("CIRCLE_LEFT_RUNNING\n");
			break;
		case 6:
			printf("CIRCLE_RIGHT_RUNNING\n");
			break;
		case 7:
			printf("CIRCLE_LEFT_OUT\n");
			break;
		case 8:
			printf("CIRCLE_RIGHT_OUT\n");
			break;
		case 9:
			printf("CIRCLE_LEFT_END\n");
			break;
		case 10:
			printf("CIRCLE_RIGHT_END\n");
			break;
		default:
			break;
		}
	}
	// cout << "Yroad_Type =" << Yroad_Type << endl;
	// cout << "YROAD_FOUND = " << YROAD_FOUND << endl;
	// cout << "YROAD_NEAR =" << YROAD_NEAR << endl;
	// 左右 L角点绘制
	Point LL(rpts0s[Lpt0_rpts0s_id][0], rpts0s[Lpt0_rpts0s_id][1]);
	circle(imgae_mask, LL, 2, Scalar(71, 97, 255), -1);
	//cout << "LLID =" << Lpt0_rpts0s_id << endl;//打印角点坐标
	//cout << "LLY =" << rpts0s[Lpt0_rpts0s_id][1] << endl;//打印角点坐标
	Point RL(rpts1s[Lpt1_rpts1s_id][0], rpts1s[Lpt1_rpts1s_id][1]);
	circle(imgae_mask, RL, 2, Scalar(71, 97, 255), -1);
	// cout << "LRX =" << rpts1s[Lpt1_rpts1s_id][0] << endl;//打印角点坐标
	// cout << "LRY =" << rpts1s[Lpt1_rpts1s_id][1] << endl;//打印角点坐标
	// cout << "LID =" << Lpt0_rpts0s_id << endl;
	// cout << "rpts1s_num =" << rpts1s_num << endl;
	// 左右 Y角点绘制
	Point LY(rpts0s[Ypt0_rpts0s_id][0], rpts0s[Ypt0_rpts0s_id][1]);
	circle(imgae_mask, LY, 2, Scalar(0, 255, 0), -1);
	Point RY(rpts1s[Ypt1_rpts1s_id][0], rpts1s[Ypt1_rpts1s_id][1]);
	circle(imgae_mask, RY, 2, Scalar(0, 255, 0), -1);


	//跟踪点点可视化
	line(imgae_mask, Point(cx - 30, cy), Point(cx - 10, cy), Scalar(0, 0, 255));
	line(imgae_mask, Point(cx + 10, cy), Point(cx + 30, cy), Scalar(0, 0, 255));
	// //  预瞄点
	circle(imgae_mask, Point(rptsn[aim_idx][0], rptsn[aim_idx][1]), 2, Scalar(0, 0, 255), -1);
	cvtColor(image_disp, Disp_gray, COLOR_GRAY2BGR);//为了显示将灰度图转化成3通道
	circle(Disp_gray, Point(Images_Cols/2,Images_Rows*0.77), 1, Scalar(0, 255, 255), -1);
	line(Disp_gray, Point(0, begin_y), Point(Disp_gray.size().width / 2 - begin_x, begin_y), Scalar(0, 0, 0));
	line(Disp_gray, Point(Disp_gray.size().width / 2 + begin_x, begin_y), Point(Disp_gray.size().width, begin_y), Scalar(0, 0, 0));
	vector<Mat>Images(2);
	Images[0] = Disp_gray;
	Images[1] = imgae_mask;
	//Images[2] = image_edge;
	namedWindow(NameWindow);
	imshowMany(NameWindow, Images);
	waitKey(1);	//延时30
	//undistortmapx[Images_Rows * 0.78][Images_Cols / 2]
}