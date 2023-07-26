#pragma once
/*********************************************************************************************************************
 * File name  : RgbImageProc.hpp-彩色摄像头预处理
 * Target core: EB FZ3B
 *  Created on: 2021年8月3日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *       
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#ifndef RGBIMGPROC_H_H
#define RGBIMGPROC_H_H
#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
using namespace cv;
using namespace std;
#define White 255
#define Black 0

#define Rize_Rows 90 
#define Rize_Cols 160

#define CorpRize_Rows 45 
#define CorpRize_Cols 80

#define RAMP_POINTS_MAX_LEN  (Rize_Rows/2)
#define ratio_X 8
#define ratio_Y 8
extern std::vector<cv::Rect> ROIs;// 注意一下ROI是针对原图的
extern Mat  image_rgb, hsv_image, red_bin_image, temp1_image, temp2_image, erode_image, dilate_image, resize_garyimage, roi_image;
extern Mat  Camera0, Camera1, cap0, cap1;
extern int rgb_servo, rgb_motor;
void Rgb2Resize_proc(Mat& image0, Mat& image1, int image_x, int image_y);
void Rgb2Gray(Mat& image0, Mat& image1);
void Rgb2LabBin_proc(Mat& image0, Mat& image1);
void Rgb2HsvBin_proc(Mat& image0, Mat& image1);
void Bin2Erode_proc(Mat& image0, Mat& image1, Mat& element);
void Bin2Dilate_proc(Mat& image0, Mat& image1, Mat& element);
int Chrek_OffTrack_Red(Mat& img1);
bool Chrek_OnTrack_Red(Mat& img0);
void Target_Cropping(Mat& img0);
void RgbImgProc(Mat& img0);
void WorkZoneOrGasStation_TASKJUDGE();
void WorkZoneOrGasStation_TASKPROC();
void WorkZoneOrGasStation_TASKCONTROL();
void WorkZone_TASK(Mat& RGBCamera0);
#endif // RGBIMGPROC_H_H