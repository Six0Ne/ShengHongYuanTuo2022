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
#pragma once
#ifndef GASSTATION_H_H
#define GASSTATION_H_H
#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
using namespace cv;
using namespace std;

enum GasStation_type_e 
{
    GASSTATION_NONE = 0, 
    GASSTATION_LEFT_FOUND,GASSTATION_RIGHT_FOUND,
    GASSTATION_LEFT_NEARDOOR, GASSTATION_RIGHT_NEARDOOR,      
    GASSTATION_LEFT_IN, GASSTATION_RIGHT_IN,  
    GASSTATION_LEFT_RUNNING, GASSTATION_RIGHT_RUNNING,              
    GASSTATION_LEFT_OUT, GASSTATION_RIGHT_OUT,            
    GASSTATION_NUM,                            
                               
};
extern enum GasStation_type_e GasStation_Type;

extern std::vector<cv::Point> GasStation_LeftPoints;
extern cv::Vec4f GasStation_Line_Param_Left;
extern std::vector<cv::Point> GasStation_RightPoints;
extern cv::Vec4f GasStation_Line_Param_Right;
extern std::vector<cv::Point> GasStation_MidPoints;
extern cv::Vec4f GasStation_Line_Param_Mid;

extern int GasStation_Enter_Cone_Left[2];
extern int GasStation_Enter_Cone_Right[2];
extern int GasStation_Exit_Cone[2];
extern int GasStation_Exit_Point[2];
extern int GasStation_INPoint[2];
extern int GasStation_BorderH;
extern bool GasStation_IfFindExit;
extern bool GasStation_IfConeNone;
extern bool GasStation_Find_INPoint;
extern int GasOutNum;
extern int GasStation_OUTPoint[2];
extern bool GasStation_Find_OUTPoint;
void Check_GasStation();
void Run_GasStation();
void GasStation_SearchEnterCone_Left(Mat& img_bin);
void GasStation_SearchMidLine_Left(Mat& img_bin);
void GasStation_SearchINPoint_Left(Mat& img_bin);
void GasStation_SearchOUTPoint_Left(Mat& img_bin);
int GasStation_Chrek_OffTrack_Red(Mat &img1);
bool GasStation_BackToRoad(Mat& img_raw);

float GasStation_StraightLineFunction_Y(int X, cv::Vec4f Line_Param);
float GasStation_StraightLineFunction_X(int Y, cv::Vec4f Line_Param);
void GasStation_Disp(Mat& DISP);
#endif // GASSTATION_H_H