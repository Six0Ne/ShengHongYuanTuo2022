#include "Six0Ne.hpp"

bool line_show_sample = true;

bool line_show_blur = false;

bool track_left = false;

bool process_start = true;

Mat  Camera0, Camera1, cap0, cap1;

Mat image_src(Size(Images_Cols, Images_Rows), CV_8UC1), adaptive_imgae(Size(Images_Cols, Images_Rows), CV_8UC1), image_gray;
void callbackSignal(int signum)
{
	int stop_servo = 0, stop_motor = 0;
	Serial_Send(&stop_servo, &stop_motor); //智能车停止运动
	servo = 0, motor = 0, rgb_servo = 0, rgb_motor = 0;
	cout << "退出程序  -->  紧急制动! " << signum << endl;
	exit(signum);
}
#if 1
Camerathread  Camera;//摄像头多线程实例化

void CameraRGB_Thread()
{
	struct  timeval RGBstart;
	struct  timeval RGBend;
	unsigned  long RGBtime = 0;
	DeeplearningClassInitial();
	signal(SIGINT, callbackSignal); //程序退出信号
	cout << "Camera3.0_Thread  -->  OK! " << endl;
	cout << "------深闳远拓 可以发车 ------" << endl;
	// string writePath = "/root/workspace/ShenHongYT_17th/src/Vision/Slope/";
	// string name;
	// int i = 193;
	while (Camera.Camera0_isOpened())//深度学习测试
	{
		gettimeofday(&RGBstart,NULL);

		if (!Camera.readCamera0(Camera0))
		{
			break;
		}
		RgbImgProc(Camera0);

		WorkZoneOrGasStation_TASKJUDGE();

		WorkZoneOrGasStation_TASKPROC();
		
		// gettimeofday(&RGBend,NULL);
		//GasStation_Disp(dilate_image);
		float time = 1000000 * (RGBend.tv_sec-RGBstart.tv_sec)+ RGBend.tv_usec-RGBstart.tv_usec;
		// Target_Cropping(Camera0);
		// imshow("RGB_DISP", roi_image);
		// waitKey(1);
		//WorkZone_TASK(Camera0);
		WorkZoneOrGasStation_TASKCONTROL();
		
		//cout << "GasOutNum =" << GasOutNum << endl;
		//cout << "GasStation_Type =" << GasStation_Type << endl;
		//cout << "TC264.eulerAngle =  " << (float)TC264.eulerAngle << endl;
		if ((WorkZone_Type != WORKZONE_NONE && WorkZone_Type != WORKZONE_LEFT_BEGIN) ||
		(GasStation_Type != GASSTATION_NONE && GasStation_Type != GASSTATION_LEFT_FOUND))
		{
			Serial_Send(&rgb_servo, &rgb_motor);//0.02ms
		}
	}
}
Mat gary2bin;
int main(int argc, char* argv[])
{
	Mat imgae_mask;
	imgae_mask.create(Images_Rows, Images_Cols, CV_8UC3);
	struct  timeval start;
	struct  timeval end;
	unsigned  long time = 0;
	Serial_Init();
	GetTc264Init(&TC264);
	PID_init(&PID_Servo, &RGBPID_Servo, &Target_Speed_PID);
	signal(SIGINT, callbackSignal); //程序退出信号
	cout << "Camera2.0_Thread  -->  OK! " << endl;
#if 1 //USB3.0摄像头线程
	thread DLDetThread(CameraRGB_Thread);
#endif
#if 1 //USB2.0摄像头线程
	while (Camera.Camera1_isOpened())
	{

		//gettimeofday(&start,NULL);
		if (!Camera.readCamera1(Camera1))
		{
			break;
		}
		
		GetTc264Data(fd, &TC264, &PID_Servo);//0.01ms				
		cvtColor(Camera1.clone(), image_src, COLOR_BGR2GRAY);//0.3ms
		Tradition_Follows();//1.5-4.5ms	
		speed_control();//速度决策	
		//motor=90;
		// servo=0;
		//buzzer = true;
		if ((WorkZone_Type == WORKZONE_NONE || WorkZone_Type == WORKZONE_LEFT_BEGIN)&&
		(GasStation_Type == GASSTATION_NONE || GasStation_Type == GASSTATION_LEFT_FOUND))
		{
			Serial_Send(&servo, &motor);//0.02ms
		}
		gettimeofday(&end,NULL);
		//cout << "Circle_Type =  " << (float)Circle_Type << endl;
		//vision_display(image_src,imgae_mask);
	}
	Camera.release();
	return 0;
#endif
}
#endif




// //DEBUG
// #if  0
// int main(int argc, char* argv[])
// {
// 	DeeplearningClassInitial();
// 	Mat imgae_mask;
// 	imgae_mask.create(Images_Rows, Images_Cols, CV_8UC3);
// 	struct  timeval start;
// 	struct  timeval end;
// 	unsigned  long time = 0;
// 	image_src = imread("/root/workspace/ShenHongYT_17th/src/Vision/ImagesData/67.jpg",IMREAD_GRAYSCALE);
// 	vector<Mat> channels;
// 	Mat resizeImgg, rayImg;
// 	while (1)
// 	{
// 		gettimeofday(&start,NULL);
// 		Tradition_Follows();//1.5-4.5ms
// 		gettimeofday(&end,NULL);
// 		vision_display(image_src,imgae_mask);
// 		time = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
// 		cout << "time = " << time << endl;	
// 	}

// 	return 0;
// }
// #endif



#if 0
int main(int argc, char* argv[])
{
	Camerathread  Camera;//摄像头多线程实例化
	//读取视频或摄像头
	// VideoCapture capture(0);
	// int fps = capture.get(CAP_PROP_FPS);
	/*capture.set(3, 320);
	capture.set(4, 200);*/
	//int fps = capture.get(CAP_PROP_FPS);
	// capture.set(3, 320);
	// capture.set(4, 200);

	Mat frame, frame_bin;
	string writePath = "/root/workspace/ShenHongYT_17th/src/Vision/ImagesData/";
	string name;
	int i = 1;

	while (true)
	{
		if (!Camera.readCamera1(Camera1))
		{
			break;
		}
		//circle(Camera1, Point(320 / 2, 200 * 0.78), 1, Scalar(0, 0, 255), -1);
		circle(Camera1, Point(Images_Cols/2,Images_Rows*0.77), 1, Scalar(0, 255, 255), -1);
		imshow("读取视频", Camera1);
		//waitKey(30);	//延时30
		if (32 == waitKey(20))
		{
			name = writePath + to_string(i) + ".jpg";
			imwrite(name, Camera1);
			cout << name << endl;
			i++;
		}
		//cout << "frame.size =" << frame.size << endl;
		//cout << "capture.exp =" << capture.get(CAP_PROP_EXPOSURE) << endl;
	}
	//waitKey(0);	//延时30
	return 0;
}
#endif

