/*********************************************************************************************************************
 * File name  : camera_set.cpp-USB2.0 USB3.0摄像头多线程读取文件
 * Target core: EB FZ3B
 *  Created on: 2021年4月25日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"

Camerathread::Camerathread()
{
	CameraInit();
}

Camerathread::~Camerathread()
{
	Camera0_IsOpen = false;
	Camera0_Thread->join();
	if (Camera0->isOpened())
	{
		Camera0->release();
	}

	delete Camera0_Thread;
	delete Camera0_Mutex;
	delete Camera0;
	delete Camera0_Frame;


	Camera1_IsOpen = false;
	Camera1_Thread->join();
	if (Camera1->isOpened())
	{
		Camera1->release();
	}

	delete Camera1_Thread;
	delete Camera1_Mutex;
	delete Camera1;
	delete Camera1_Frame;
}

void Camerathread::CameraInit()
{
	cout << "------摄像头自检ing------" << endl;
	//自检
	Camera0 = new cv::VideoCapture(0);
	Camera0->open(0, CV_CAP_V4L2);
	Camera0->set(cv::CAP_PROP_FRAME_WIDTH, 640);
	Camera0->set(cv::CAP_PROP_FRAME_HEIGHT, 360);
	Camera0_IsOpen = true;
	Camera0_Frame = new cv::Mat(360, 640, CV_8UC3);

	//m_pCapture->set(cv::CAP_PROP_FPS, 30);
	Camera1 = new cv::VideoCapture(1);
	Camera1->open(1, CV_CAP_V4L2);
	Camera1->set(cv::CAP_PROP_FRAME_WIDTH, 320);
	Camera1->set(cv::CAP_PROP_FRAME_HEIGHT, 200);
	Camera1_IsOpen = true;
	Camera1_Frame = new cv::Mat(200, 320, CV_8UC3);
	if (Camera1->get(cv::CAP_PROP_FPS) < 100)
	{
		delete Camera0;
		delete Camera1;
		delete Camera0_Frame;
		delete Camera1_Frame;
		Camera0 = new cv::VideoCapture(1);
		Camera0->open(1, CV_CAP_V4L2);
		Camera0->set(cv::CAP_PROP_FRAME_WIDTH, 640);
		Camera0->set(cv::CAP_PROP_FRAME_HEIGHT, 360);
		Camera0_Frame = new cv::Mat(360, 640, CV_8UC3);
		//m_pCapture->set(cv::CAP_PROP_FPS, 30);
		Camera1 = new cv::VideoCapture(0);
		Camera1->open(0, CV_CAP_V4L2);
		Camera1->set(cv::CAP_PROP_FRAME_WIDTH, 320);
		Camera1->set(cv::CAP_PROP_FRAME_HEIGHT, 200);
		Camera1_Frame = new cv::Mat(200, 320, CV_8UC3);
	}

	Camera0_Mutex = new std::mutex();
	Camera0_Thread = new std::thread(&Camerathread::Camera0Frame, this);


	Camera1_Mutex = new std::mutex();
	Camera1_Thread = new std::thread(&Camerathread::Camera1Frame, this);
	cout << " Camera0  " << Camera0->get(CAP_PROP_FPS) << endl;
	cout << " Camera1  " << Camera1->get(CAP_PROP_FPS) << endl;
	if (Camera0->get(CAP_PROP_FPS) == 0) cout << " Camera0 --> 404  " << Camera0->get(CAP_PROP_FPS) << endl;
	if (Camera1->get(CAP_PROP_FPS) == 0) cout << " Camera1 --> 404  " << Camera0->get(CAP_PROP_FPS) << endl;
	cout << "------摄像头自检完毕------" << endl;
	cout << "------摄像头预热ing------" << endl;
	for (int i = 0; i < 10; i++)                               				// 循环更新摄像头帧，为摄像头预热
	{
		(*Camera0) >> (*Camera0_Frame);
		(*Camera1) >> (*Camera1_Frame);

	}
	cout << "------摄像头预热完毕------" << endl;

}

void Camerathread::Camera0Frame()
{
	cv::Mat frameBuff;
	while (Camera0_IsOpen)
	{
		(*Camera0) >> frameBuff;
		if (Camera0_Mutex->try_lock())
		{
			frameBuff.copyTo(*Camera0_Frame);
			Camera0_Mutex->unlock();
		}
	}
}

void Camerathread::Camera1Frame()
{
	cv::Mat frameBuff;
	while (Camera1_IsOpen)
	{
		(*Camera1) >> frameBuff;
		if (Camera1_Mutex->try_lock())
		{
			frameBuff.copyTo(*Camera1_Frame);
			Camera1_Mutex->unlock();
		}
	}
}

bool Camerathread::readCamera0(cv::Mat& frame0)
{
	if (Camera0_Frame->empty())
	{
		Camera0_IsOpen = false;
	}
	else {
		Camera0_Mutex->lock();
		Camera0_Frame->copyTo(frame0);
		Camera0_Mutex->unlock();
	}
	return Camera0_IsOpen;
}
bool Camerathread::readCamera1(cv::Mat& frame1)
{
	if (Camera1_Frame->empty())
	{
		Camera1_IsOpen = false;
	}
	else {
		Camera1_Mutex->lock();
		Camera1_Frame->copyTo(frame1);
		Camera1_Mutex->unlock();
	}
	return Camera1_IsOpen;
}

