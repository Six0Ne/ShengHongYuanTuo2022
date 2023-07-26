#pragma once
#ifndef IMGPROC_H_H
#define IMGPROC_H_H
#include <string.h>
#include <opencv2/opencv.hpp>
#include "Utils.hpp"
#include <stdbool.h>
#include <stdint.h>
#include "Common.hpp"
using namespace cv;
using namespace std;
#define Images_Rows 200 
#define Images_Cols 320


// extern enum yroad_type_e yroad_type;
// extern enum garage_type_e garage_type;
// extern enum circle_type_e circle_type;
// extern enum cross_type_e cross_type;
typedef struct image
{
	uint8_t* data;
	uint32_t width;
	uint32_t height;
	uint32_t step;
} image_t;

typedef struct fimage
{
	float* data;
	uint32_t width;
	uint32_t height;
	uint32_t step;
} fimage_t;

#define AT_IMAGE(img, x, y)          ((img)->data[(y)*(img)->step+(x)])//���ûҶ�ֵ

//#define AT_IMAGE(img, x, y)          ((img)->data[(img)->step+(x)])
#define AT_IMAGE_CLIP(img, x, y)     AT_IMAGE(img, clip(x, 0, (img)->width-1), clip(y, 0, (img)->height-1))

//#define DEF_IMAGE(ptr, w, h)         {.data=ptr, .width=w, .height=h, .step=w}//�����Ӧ����c���ԵĽṹ���ʼ��������c����.���������ĸ�ֵ
#define DEF_IMAGE(ptr, w, h)         {ptr, w, h, w}
#define ROI_IMAGE(img, X1, Y1, w, h) {.data=&AT_IMAGE(img, X1, Y1), .width=w, .height=h, .step=img.width}
#define ROAD_WIDTH      (0.40) //·��0.40cm
#define POINTS_MAX_LEN  (Images_Rows)
#define FAR_POINTS_MAX_LEN  (POINTS_MAX_LEN)
#define Generic_POINTS_MAX_LEN  (POINTS_MAX_LEN)
#define CONE_POINTS_MAX_LEN  (POINTS_MAX_LEN)
extern uint8_t gray_image[Images_Rows][Images_Cols];
extern image_t img_raw;//


extern bool line_show_sample;
extern bool line_show_blur;
extern bool track_left;

extern float angle;

extern int servo, motor;
extern int aim_idx,aim_idx_near;
extern Mat image_src;


// 原图左右边线
extern int ipts0[POINTS_MAX_LEN][2];
extern int ipts1[POINTS_MAX_LEN][2];
extern int ipts0_num, ipts1_num;
// 变换后左右边线
extern float rpts0[POINTS_MAX_LEN][2];
extern float rpts1[POINTS_MAX_LEN][2];
extern int rpts0_num, rpts1_num;
// 变换后左右边线+滤波
extern float rpts0b[POINTS_MAX_LEN][2];
extern float rpts1b[POINTS_MAX_LEN][2];
extern int rpts0b_num, rpts1b_num;
// 变换后左右边线+等距采样
extern float rpts0s[POINTS_MAX_LEN][2];
extern float rpts1s[POINTS_MAX_LEN][2];
extern int rpts0s_num, rpts1s_num;
// 左右边线局部角度变化率
extern float rpts0a[POINTS_MAX_LEN];
extern float rpts1a[POINTS_MAX_LEN];
extern int rpts0a_num, rpts1a_num;
// 左右边线局部角度变化率+非极大抑制
extern float rpts0an[POINTS_MAX_LEN];
extern float rpts1an[POINTS_MAX_LEN];
extern int rpts0an_num, rpts1an_num;
// 左/右中线
extern float rptsc0[POINTS_MAX_LEN][2];
extern float rptsc1[POINTS_MAX_LEN][2];
extern int rptsc0_num, rptsc1_num;
// 中线
extern float(*rpts)[2];
extern int rpts_num;
// 归一化中线
extern float rptsn[POINTS_MAX_LEN][2];
extern int rptsn_num;

// Y角点
extern int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
extern bool Ypt0_found, Ypt1_found;

// L角点
extern int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
extern bool Lpt0_found, Lpt1_found;



extern bool is_straight0, is_straight1;
extern bool is_longstraight0, is_longstraight1;
//巡线模式
enum track_type_e 
{
	TRACK_LEFT,
	TRACK_RIGHT,
};
extern enum track_type_e Track_Type;



extern float cx;
extern float cy;

extern bool is_turn0, is_turn1;
extern float thres;
extern float block_size ;
extern float clip_value ;
extern float begin_x;
extern float begin_y;
extern float line_blur_kernel;
extern float pixel_per_meter;
extern float sample_dist;
extern float angle_dist;
extern float far_rate;
extern float aim_distance;
extern bool adc_cross;

int clip(int x, int low, int up);

float fclip(float x, float low, float up);

void MatToArry(Mat& img0, uint8* img1);

void ArryToMat(uint8* img, Mat& img0);

void MatToimage_t(Mat& img0, image_t* img);

void Image_tToMat(image_t* img, Mat& img0);


uint8 MyOSTU(Mat& img1, int star_y, int width);

void Clone_Image(image_t* img0, image_t* img1);


void Clear_Image(image_t* img);


void Threshold(image_t* img0, image_t* img1, uint8_t thres, uint8_t low_value, uint8_t high_value);


void Adaptive_Threshold(image_t* img0, image_t* img1, int block_size, int down_value, uint8_t low_value, uint8_t high_value);

void Adaptive_ThresholdMat(Mat& img0, Mat& img1, int block_size, int down_value, uint8_t low_value, uint8_t high_value);

void Image_And(image_t* img0, image_t* img1, image_t* img2);


void Image_Or(image_t* img0, image_t* img1, image_t* img2);


void minpool2(image_t* img0, image_t* img1);


void Sobel3(image_t* img0, image_t* img1);


void Erode3(image_t* img0, image_t* img1);


void Dilate3(image_t* img0, image_t* img1);



void Remap(image_t* img0, image_t* img1, fimage_t* mapx, fimage_t* mapy);


void FindLine_LefThand_Adaptive(image_t* img, int block_size, int clip_value, int x, int y, int pts[][2], int* num);

void FindLine_LefThand_Mat(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num);

void FindLine_LefThand_Mat(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num);

void FindLine_LefThand_Bluecloth(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num);

void FindLine_RighThand_Adaptive(image_t* img, int block_size, int clip_value, int x, int y, int pts[][2], int* num);

void FindLine_RighThand_Mat(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num);

void FindLine_RighThand_Bluecloth(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num);

uint16_t GetOSTUThreshold(image_t* img, uint8_t MinThreshold, uint8_t MaxThreshold);


void Blur_Points(float pts_in[][2], int num, float pts_out[][2], int kernel);


void Resample_Points(float pts_in[][2], int num1, float pts_out[][2], int* num2, float dist);


void Resample_Points2(float pts_in[][2], int num1, float pts_out[][2], int* num2, float dist);

void Local_Angle_Points(float pts_in[][2], int num, float angle_out[], int dist);


void Nms_Angle(float angle_in[], int num, float angle_out[], int kernel);


void Track_LeftLine(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);


void Track_RightLine(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);



void MatToArry(Mat& img0, uint8* img1);

void ArryToMat(uint8* img, Mat& img0);

void MatToimage_t(Mat& img0, image_t* img);

void Image_tToMat(image_t* img, Mat& img0);


void Process_GaryImage();


void Process_Corners();


void Tradition_Follows();

#endif // IMGPROC_H