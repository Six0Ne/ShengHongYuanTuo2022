#ifndef DEEPLEARNINGDETECTION_HPP
#define DEEPLEARNINGDETECTION_HPP

#include <iostream>
#include <vector>
#include "paddle_api.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
typedef struct DetClass
{
	int box_num;
	int boxmid_x[20];
	int boxmid_y[20];
} DetClass;
typedef struct deeplearningclass
{
	int WorkZone_Num;
	int GasStation_Num;
	int Ramp_Num;
	int WriteOne_Num;
	int WriteTwo_Num;
	void ClearALL()
	{
		WorkZone_Num = 0;
		GasStation_Num = 0;
		Ramp_Num = 0;
		WriteOne_Num = 0;
		WriteTwo_Num = 0;
	}
	void ClearWorkZone()
	{
		WorkZone_Num = 0;
	}
	void ClearGasStation()
	{
		GasStation_Num = 0;
	}
	void ClearRamp()
	{
		Ramp_Num = 0;
	}
	void ClearWriteOne()
	{
		WriteOne_Num = 0;
	}
	void ClearWriteTwo()
	{
		WriteTwo_Num = 0;
	}
} deeplearning;
enum deeplearning_type_e
{
	NoResults = 0, //无结果
	WorkZone,   //施工区
	GasStation,     //加油站
	Ramp,           //坡道
	WriteOne,          //1    
	WriteTwo,             //2                          
};
extern enum deeplearning_type_e DeepLearning_Type;
extern float* Detection_Output_Data;
extern int Detection_Output_Size;
extern Mat img_raw0;
extern deeplearning TrackRedCla;
#define DEF_DLC(BusyArea_num, GasStation_num, Ramp_num,WriteOne_num,WriteTwo_num)         {BusyArea_num, GasStation_num, Ramp_num, WriteOne_num,WriteTwo_num}
void DeeplearningDetectionInitial();
void DeeplearningDetection(Mat& img_raw0);
void Target_cropping(Mat& img0);
void DeeplearningClassInitial();
void DeeplearningClass(Mat& img_raw0,Rect rect);
void DeeplearningDisplay(Mat& img_raw0_mat);

#endif /* DEEPLEARNINGDETECTION_HPP */