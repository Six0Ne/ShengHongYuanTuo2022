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
#pragma once
#ifndef WORKZOON_H_H
#define WORKZOON_H_H
#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
using namespace cv;
using namespace std;

enum WorkZone_type_e 
{
    WORKZONE_NONE = 0, 
    WORKZONE_FOUND,
    WORKZONE_LEFT_BEGIN, WORKZONE_RIGHT_BEGIN,
    WORKZONE_LEFT_NEARDOOR, WORKZONE_RIGHT_NEARDOOR,      
    WORKZONE_LEFT_IN, WORKZONE_RIGHT_IN,  
    WORKZONE_LEFT_RUNNING, WORKZONE_RIGHT_RUNNING,              
    WORKZONE_LEFT_OUT, WORKZONE_RIGHT_OUT,            
    WORKZONE_NUM,                            
                               
};
extern enum WorkZone_type_e WorkZone_Type;

extern std::vector<cv::Point> WorkZone_LeftPoints;
extern cv::Vec4f WorkZone_Line_Param_Left;
extern std::vector<cv::Point> WorkZone_RightPoints;
extern cv::Vec4f WorkZone_Line_Param_Right;
extern std::vector<cv::Point> WorkZone_MidPoints;
extern cv::Vec4f WorkZone_Line_Param_Mid;

extern int WorkZone_Enter_Cone_Left[2];
extern int WorkZone_Enter_Cone_Right[2];
extern int WorkZone_Exit_Cone[2];
extern int WorkZone_Exit_Point[2];
extern int WorkZone_Red_Sign_Position[2];
extern int WorkZone_INPoint[2];
extern int WorkZone_BorderH;
extern bool WorkZone_IfFindExit;
extern bool WorkZone_IfConeNone;
extern bool WorkZone_Find_INPoint;

extern int WorkZone_OUTPoint[2];
extern bool WorkZone_Find_OUTPoint;
void Check_WorkZone();
void Run_WorkZone();
void WorkZone_Control();
void WorkZone_SearchEnterCone_Left(Mat& img_bin);
void WorkZone_SearchMidLine_Left(Mat& img_bin);
void WorkZone_SearchINPoint_Left(Mat& img_bin);
void WorkZone_SearchOUTPoint_Left(Mat& img_bin);
bool WorkZone_Chrek_OnTrack_Red(Mat &img0);
int WorkZone_Chrek_OffTrack_Red(Mat &img1);
bool WorkZone_BackToRoad(Mat& img_raw);
void WorkZone_Disp(Mat& DISP);

float WorkZone_StraightLineFunction_Y(int X, cv::Vec4f Line_Param);
float WorkZone_StraightLineFunction_X(int Y, cv::Vec4f Line_Param);
#endif // CONEBUCKET_H