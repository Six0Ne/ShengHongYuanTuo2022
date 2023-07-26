/*********************************************************************************************************************
 * File name  : garage.cpp-传统循迹控制逻辑
 * Target core: EB FZ3B
 *  Created on: 2021年4月25日
*   Team name: 深闳远拓
 *      Author: Six0Ne
 *            （优化16届上海交通大学开源方案）
 *                                           CUCJUMITS_17_完全模型
 *                                        集美大学诚毅学院电子创新实验室
 *                                  "In me the tiger sniffs the rose."
 **********************************************************************************************************************/
#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"

#define AT                  AT_IMAGE
#define AT_CLIP(img, x, y)  AT_IMAGE((img), clip((x), 0, (img)->width-1), clip((y), 0, (img)->height-1));
extern int clip(int x, int low, int up);

enum track_type_e Track_Type = TRACK_LEFT;//巡线模式

// 原图左右边线
int ipts0[POINTS_MAX_LEN][2];
int ipts1[POINTS_MAX_LEN][2];
int ipts0_num, ipts1_num;//左右边线像素点数
// 变换后左右边线
float rpts0[POINTS_MAX_LEN][2];
float rpts1[POINTS_MAX_LEN][2];
int rpts0_num, rpts1_num;
// 变换后左右边线+滤波
float rpts0b[POINTS_MAX_LEN][2];
float rpts1b[POINTS_MAX_LEN][2];
int rpts0b_num, rpts1b_num;
// 变换后左右边线+等距采样
float rpts0s[POINTS_MAX_LEN][2];
float rpts1s[POINTS_MAX_LEN][2];
int rpts0s_num, rpts1s_num;
// 左右边线局部角度变化率
float rpts0a[POINTS_MAX_LEN];
float rpts1a[POINTS_MAX_LEN];
int rpts0a_num, rpts1a_num;
// 左右边线局部角度变化率+非极大抑制
float rpts0an[POINTS_MAX_LEN];
float rpts1an[POINTS_MAX_LEN];
int rpts0an_num, rpts1an_num;
// 左/右中线
float rptsc0[POINTS_MAX_LEN][2];
float rptsc1[POINTS_MAX_LEN][2];
int rptsc0_num, rptsc1_num;
// 中线
float(*rpts)[2];
int rpts_num;
// 归一化中线
float rptsn[POINTS_MAX_LEN][2];
int rptsn_num;

// Y角点
int Ypt0_rpts0s_id, Ypt1_rpts1s_id;
bool Ypt0_found, Ypt1_found;

// L角点
int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
bool Lpt0_found, Lpt1_found;

// 直道
bool is_straight0, is_straight1;

// 长直道
bool is_longstraight0, is_longstraight1;

// 弯道
bool is_turn0, is_turn1;
void MatToArry(Mat& img0, uint8* img1)//opencv 把Mat数据类型转换成数组储存，耗时30us  实测 操作数组比操作Mat快上不少 
{
	unsigned int i, j;
	unsigned char* ptr;
	for (i = 0; i < Images_Rows; i++)//从近到远扫
	{
		ptr = img0.ptr<uchar>(i);//利用指针去访问Mat中的元素
		for (j = 0; j < Images_Cols; j++)//从近到远扫
		{
			img1[(i)*Images_Cols + j] = ptr[j];
		}
	}
}
void ArryToMat(uint8* img1, Mat& img0)//opencv 把Mat数据类型转换成数组储存，耗时30us 
{
	unsigned int i, j;
	unsigned char* ptr;
	for (i = 0; i < Images_Rows; i++)//从近到远扫
	{
		ptr = img0.ptr<uchar>(i);//利用指针去访问Mat中的元素
		for (j = 0; j < Images_Cols; j++)//从近到远扫
		{
			ptr[j] = img1[(i)*Images_Cols + j];
		}
	}

}
void MatToimage_t(Mat& img0, image_t* img)
{
	unsigned int i, j;
	unsigned char* ptr;
	for (i = 0; i < Images_Rows; i++)//从近到远扫
	{
		ptr = img0.ptr<uchar>(i);//利用指针去访问Mat中的元素
		for (j = 0; j < Images_Cols; j++)//从近到远扫
		{
			AT_IMAGE(img, j, i) = ptr[j];
		}
	}
}
void Image_tToMat(image_t* img, Mat& img0)
{
	unsigned int i, j;
	unsigned char* ptr;
	for (i = 0; i < Images_Rows; i++)//从近到远扫
	{
		ptr = img0.ptr<uchar>(i);//利用指针去访问Mat中的元素
		for (j = 0; j < Images_Cols; j++)//从近到远扫
		{
			ptr[j] = AT_IMAGE(img, j, i);
		}
	}
}
void Clone_Image(image_t* img0, image_t* img1)//深拷贝图像
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img0->width == img1->width && img0->height == img1->height);
	assert(img0 != img1 && img0->data != img1->data);

	//上面是断言，不满足断言条件则会中止程序，输出报错信息  一般断言只会在debug版本出现，发行版是不会出现的。
	if (img0->width == img0->step && img1->width == img1->step)
	{
		memcpy(img1->data, img0->data, (img0->width * img0->height));// 从存储区 str2 复制 n 个字节到存储区 str1。
		//第一个形参是指向用于存储复制内容的目标数组，类型强制转换为 void* 指针。
		//第二个形参是指向要复制的数据源，类型强制转换为 void* 指针。
		//第三个形参是要被复制的字节数。
	}
	else
	{
		for (int y = 0; y < img0->height; y++)
		{
			memcpy(&AT(img1, 0, y), &AT(img0, 0, y), img0->width);
		}
	}
}

void Clear_Image(image_t* img)
{
	assert(img && img->data);
	if (img->width == img->step)
	{
		memset(img->data, 0, (uint64_t)(img->width) * (img->height));//memset() 的作用是在一段内存块中填充某个给定的值。因为它只能填充一个值，所以该函数的初始化为原始初始化，无法将变量初始化为程序中需要的数据。用memset初始化完后，后面程序中再向该内存空间中存放需要的数据。
	}
	else
	{
		for (int y = 0; y < img->height; y++)
		{
			memset(&AT(img, 0, y), 0, img->width);
		}
	}
}

// 固定阈值二值化
void Threshold(image_t* img0, image_t* img1, uint8_t thres, uint8_t low_value, uint8_t high_value)
{
	//利用断言在函数开始处检验传入参数的合法性如：
	assert(img0 && img0->data);
	assert(img1 && img1->data);

	assert(img0->width == img1->width && img0->height == img1->height);//长宽高要一样

	// 先遍历y后遍历x比较cache-friendly(缓存友好)
	for (int y = 0; y < img0->height; y++)
	{
		for (int x = 0; x < img0->width; x++)
		{
			AT(img1, x, y) = AT(img0, x, y) < thres ? low_value : high_value;
		}
	}
}



//float block_size = 7;
//float clip_value = 2;
//findline_lefthand_adaptive(&img_raw, block_size, clip_value, x1, y1, ipts0, &ipts0_num);


// 自适应阈值二值化
void Adaptive_Threshold(image_t* img0, image_t* img1, int block_size, int down_value, uint8_t low_value, uint8_t high_value)
{
	//利用断言在函数开始处检验传入参数的合法性如：
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img0->data != img1->data);
	assert(img0->width == img1->width && img0->height == img1->height);//两个图像是否长宽高一样
	assert(block_size > 1 && block_size % 2 == 1);//种子模板是否大于1，是否不被二整除

	int half = block_size / 2;//3
	// 先遍历y后遍历x比较cache-friendly
	for (int y = 0; y < img0->height; y++)
	{
		for (int x = 0; x < img0->width; x++)
		{
			int thres_value = 0;
			for (int dy = -half; dy <= half; dy++)
			{
				for (int dx = -half; dx <= half; dx++)
				{
					thres_value += AT_CLIP(img0, x + dx, y + dy);
					//#define AT_CLIP(img, x, y)  AT_IMAGE((img), clip((x), 0, (img)->width-1), clip((y), 0, (img)->height-1));
				}
			}
			thres_value /= block_size * block_size;
			thres_value -= down_value;
			AT(img1, x, y) = AT(img0, x, y) < thres_value ? low_value : high_value;//三目运算符
		}
	}
}
// Mat自适应阈值二值化
void Adaptive_ThresholdMat(Mat& img0, Mat& img1, int block_size, int down_value, uint8_t low_value, uint8_t high_value)
{
	//利用断言在函数开始处检验传入参数的合法性如：
	// assert(img0 && img0->data);
	// assert(img1 && img1->data);
	// assert(img0->data != img1->data);
	//assert(img0.cols == img1.cols && img0.rows == img1.rows);//两个图像是否长宽高一样
	assert(block_size > 1 && block_size % 2 == 1);//种子模板是否大于1，是否不被二整除

	int half = block_size / 2;//3
	// 先遍历y后遍历x比较cache-friendly
	for (int y = 0; y < img0.rows; y++)
	{
		for (int x = 0; x < img0.cols; x++)
		{
			int thres_value = 0;
			for (int dy = -half; dy <= half; dy++)
			{
				for (int dx = -half; dx <= half; dx++)
				{
					thres_value += img0.at<uchar>(y + dy, x + dx);

					//#define AT_CLIP(img, x, y)  AT_IMAGE((img), clip((x), 0, (img)->width-1), clip((y), 0, (img)->height-1));
				}
			}
			thres_value /= block_size * block_size;
			thres_value -= down_value;
			img1.at<uchar>(y, x) = img0.at<uchar>(y, x) < thres_value ? low_value : high_value;//三目运算符
		}
	}
}

// 图像逻辑与
void Image_And(image_t* img0, image_t* img1, image_t* img2)
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img2 && img2->data);
	assert(img0->width == img1->width && img0->height == img1->height);
	assert(img0->width == img2->width && img0->height == img2->height);

	// 先遍历y后遍历x比较cache-friendly
	for (int y = 0; y < img0->height; y++) {
		for (int x = 0; x < img0->width; x++) {
			AT(img2, x, y) = (AT(img0, x, y) == 0 || AT(img1, x, y) == 0) ? 0 : 255;
		}
	}
}

// 图像逻辑或
void Image_Or(image_t* img0, image_t* img1, image_t* img2)
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img2 && img2->data);
	assert(img0->width == img1->width && img0->height == img1->height);
	assert(img0->width == img2->width && img0->height == img2->height);

	// 先遍历y后遍历x比较cache-friendly
	for (int y = 0; y < img0->height; y++) {
		for (int x = 0; x < img0->width; x++) {
			AT(img2, x, y) = (AT(img0, x, y) == 0 && AT(img1, x, y) == 0) ? 0 : 255;
		}
	}
}

// 2x2最小池化(赛道边界是黑色，最小池化可以较好保留赛道边界)
void Minpool2(image_t* img0, image_t* img1)
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img0->width / 2 == img1->width && img0->height / 2 == img1->height);
	assert(img0 != img1 && img0->data != img1->data);

	uint8_t min_value;
	// 先遍历y后遍历x比较cache-friendly
	for (int y = 1; y < img0->height; y += 2) {
		for (int x = 1; x < img0->width; x += 2) {
			min_value = 255;
			if (AT(img0, x, y) < min_value) min_value = AT(img0, x, y);
			if (AT(img0, x - 1, y) < min_value) min_value = AT(img0, x - 1, y);
			if (AT(img0, x, y - 1) < min_value) min_value = AT(img0, x, y - 1);
			if (AT(img0, x - 1, y - 1) < min_value) min_value = AT(img0, x - 1, y - 1);
			AT(img1, x / 2, y / 2) = min_value;
		}
	}
}

// 图像滤波降噪
void Blur(image_t* img0, image_t* img1, uint32_t kernel)
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img0->width == img1->width && img0->height == img1->height);
	assert(img0 != img1 && img0->data != img1->data);

	// 先遍历y后遍历x比较cache-friendly
	for (int y = 1; y < img0->height - 1; y++) {
		for (int x = 1; x < img0->width - 1; x++) {
			AT(img1, x, y) = (1 * AT(img0, x - 1, y - 1) + 2 * AT(img0, x, y - 1) + 1 * AT(img0, x + 1, y - 1) +
				2 * AT(img0, x - 1, y) + 4 * AT(img0, x, y) + 2 * AT(img0, x + 1, y) +
				1 * AT(img0, x - 1, y + 1) + 2 * AT(img0, x, y + 1) + 1 * AT(img0, x + 1, y + 1)) / 16;
		}
	}
}

// 3x3sobel边缘提取
void Sobel3(image_t* img0, image_t* img1)
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img0->width == img1->width && img0->height == img1->height);
	assert(img0 != img1 && img0->data != img1->data);

	int gx, gy;
	// 先遍历y后遍历x比较cache-friendly    
	for (int y = 1; y < img0->height - 1; y++) {
		for (int x = 1; x < img0->width - 1; x++) {
			gx = (-1 * AT(img0, x - 1, y - 1) + 1 * AT(img0, x + 1, y - 1) +
				-2 * AT(img0, x - 1, y) + 2 * AT(img0, x + 1, y) +
				-1 * AT(img0, x - 1, y + 1) + 1 * AT(img0, x + 1, y + 1)) / 4;
			gy = (1 * AT(img0, x - 1, y - 1) + 2 * AT(img0, x, y - 1) + 1 * AT(img0, x + 1, y - 1) +
				-1 * AT(img0, x - 1, y + 1) - 2 * AT(img0, x, y + 1) - 1 * AT(img0, x + 1, y + 1)) / 4;
			AT(img1, x, y) = (abs(gx) + abs(gy)) / 2;
		}
	}
}

// 3x3腐蚀
void Erode3(image_t* img0, image_t* img1)
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img0->width == img1->width && img0->height == img1->height);
	assert(img0 != img1 && img0->data != img1->data);

	int min_value;
	// 先遍历y后遍历x比较cache-friendly    
	for (int y = 1; y < img0->height - 1; y++) {
		for (int x = 1; x < img0->width - 1; x++) {
			min_value = 255;
			for (int dy = -1; dy <= 1; dy++) {
				for (int dx = -1; dx <= 1; dx++) {
					if (AT(img0, x + dx, y + dy) < min_value) min_value = AT(img0, x + dx, y + dy);
				}
			}
			AT(img1, x, y) = min_value;
		}
	}
}

// 3x3膨胀
void Dilate3(image_t* img0, image_t* img1)
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(img0->width == img1->width && img0->height == img1->height);
	assert(img0 != img1 && img0->data != img1->data);

	int max_value;
	// 先遍历y后遍历x比较cache-friendly    
	for (int y = 1; y < img0->height - 1; y++) {
		for (int x = 1; x < img0->width - 1; x++) {
			max_value = 0;
			for (int dy = -1; dy <= 1; dy++) {
				for (int dx = -1; dx <= 1; dx++) {
					if (AT(img0, x + dx, y + dy) > max_value) max_value = AT(img0, x + dx, y + dy);
				}
			}
			AT(img1, x, y) = max_value;
		}
	}
}

//
void Remap(image_t* img0, image_t* img1, fimage_t* mapx, fimage_t* mapy)
{
	assert(img0 && img0->data);
	assert(img1 && img1->data);
	assert(mapx && mapx->data);
	assert(mapy && mapy->data);
	assert(img0 != img1 && img0->data != img1->data);
	assert(img0->width == img1->width && img0->height == img1->height);
	assert(mapx->width == mapy->width && mapx->height == mapy->height);
	assert(img0->width == mapx->width && img0->height == mapx->height);

	// 先遍历y后遍历x比较cache-friendly    
	for (int y = 1; y < img0->height - 1; y++) {
		for (int x = 1; x < img0->width - 1; x++) {
			AT(img1, x, y) = AT(img0, (int)(AT(mapx, x, y) + 0.5), (int)(AT(mapy, x, y) + 0.5));
		}
	}
}

/* 前进方向定义：
 *   0
 * 3   1
 *   2
 */                          //X    Y
const int dir_front[4][2] = { {0,  -1},//正前方
							  {1,  0},//正右方
							  {0,  1},//正下方
							  {-1, 0} };//正左方
const int dir_frontleft[4][2] = { {-1, -1},
								  {1,  -1},
								  {1,  1},
								  {-1, 1} };
const int dir_frontright[4][2] = { {1,  -1},
								   {1,  1},
								   {-1, 1},
								   {-1, -1} };

// 左手迷宫巡线
void FindLine_LefThand_Adaptive(image_t* img, int block_size, int clip_value, int x, int y, int pts[][2], int* num)
{
	assert(img && img->data);
	assert(num && *num >= 0);
	assert(block_size > 1 && block_size % 2 == 1);
	int half = block_size / 2;
	int step = 0, dir = 0, turn = 0;
	while (step < *num && half < x && x < img->width - half - 1 && half < y && y < img->height - half - 1 && turn < 4)
	{
		int local_thres = 0;
		for (int dy = -half; dy <= half; dy++)
		{
			for (int dx = -half; dx <= half; dx++)
			{
				local_thres += AT(img, x + dx, y + dy);
			}
		}
		local_thres /= block_size * block_size;
		local_thres -= clip_value;

		int current_value = AT(img, x, y);
		int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
		int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);
		if (front_value < local_thres)
		{
			dir = (dir + 1) % 4;
			turn++;
		}
		else if (frontleft_value < local_thres)
		{
			x += dir_front[dir][0];
			y += dir_front[dir][1];
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
		else
		{
			x += dir_frontleft[dir][0];
			y += dir_frontleft[dir][1];
			dir = (dir + 3) % 4;
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
	}
	*num = step;
}
// Mat左手迷宫巡线
void FindLine_LefThand_Mat(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num)
{
	//assert(img && img->data);
	assert(num && *num >= 0);
	assert(block_size > 1 && block_size % 2 == 1);
	int half = block_size / 2;
	int step = 0, dir = 0, turn = 0;
	while (step < *num && half < x && x < img.cols - half - 1 && half < y && y < img.rows - half - 1 && turn < 4)
	{
		int local_thres = 0;
		for (int dy = -half; dy <= half; dy++)
		{
			for (int dx = -half; dx <= half; dx++)
			{
				local_thres += img.at<uchar>(y + dy, x + dx);
				//local_thres += AT(img, x + dx, y + dy);
			}
		}
		local_thres /= block_size * block_size;
		local_thres -= clip_value;

		int current_value = img.at<uchar>(y, x);
		int front_value = img.at<uchar>(y + dir_front[dir][1], x + dir_front[dir][0]);
		int frontleft_value = img.at<uchar>(y + dir_frontleft[dir][1], x + dir_frontleft[dir][0]);
		// int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
		// int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);
		if (front_value < local_thres)
		{
			dir = (dir + 1) % 4;
			turn++;
		}
		else if (frontleft_value < local_thres)
		{
			x += dir_front[dir][0];
			y += dir_front[dir][1];
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
		else
		{
			x += dir_frontleft[dir][0];
			y += dir_frontleft[dir][1];
			dir = (dir + 3) % 4;
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
	}
	*num = step;
}
// Mat左手迷宫巡线
void FindLine_LefThand_Bluecloth(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num)
{
	//assert(img && img->data);
	assert(num && *num >= 0);
	assert(block_size > 1 && block_size % 2 == 1);
	int half = block_size / 2;
	int step = 0, dir = 0, turn = 0;
	while (step < *num && half < x && x < img.cols - half - 1 && half < y && y < img.rows - half - 1 && turn < 4)
	{
		int local_thres = 0;
		for (int dy = -half; dy <= half; dy++)
		{
			for (int dx = -half; dx <= half; dx++)
			{
				local_thres += img.at<uchar>(y + dy, x + dx);
				//local_thres += AT(img, x + dx, y + dy);
			}
		}
		local_thres /= block_size * block_size;
		local_thres -= clip_value;

		int current_value = img.at<uchar>(y, x);
		int front_value = img.at<uchar>(y + dir_front[dir][1], x + dir_front[dir][0]);
		int frontleft_value = img.at<uchar>(y + dir_frontleft[dir][1], x + dir_frontleft[dir][0]);
		// int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
		// int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);
		if (front_value > local_thres)
		{
			dir = (dir + 1) % 4;
			turn++;
		}
		else if (frontleft_value > local_thres)
		{
			x += dir_front[dir][0];
			y += dir_front[dir][1];
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
		else
		{
			x += dir_frontleft[dir][0];
			y += dir_frontleft[dir][1];
			dir = (dir + 3) % 4;
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
	}
	*num = step;
}
// 右手迷宫巡线
void FindLine_RighThand_Adaptive(image_t* img, int block_size, int clip_value, int x, int y, int pts[][2], int* num)
{
	assert(img && img->data);
	assert(num && *num >= 0);
	assert(block_size > 1 && block_size % 2 == 1);
	int half = block_size / 2;
	int step = 0, dir = 0, turn = 0;
	while (step < *num && 0 < x && x < img->width - 1 && 0 < y && y < img->height - 1 && turn < 4)
	{
		int local_thres = 0;
		for (int dy = -half; dy <= half; dy++)
		{
			for (int dx = -half; dx <= half; dx++)
			{
				local_thres += AT(img, x + dx, y + dy);
			}
		}
		local_thres /= block_size * block_size;
		local_thres -= clip_value;

		int current_value = AT(img, x, y);
		int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
		int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
		if (front_value < local_thres)
		{
			dir = (dir + 3) % 4;
			turn++;
		}
		else if (frontright_value < local_thres)
		{
			x += dir_front[dir][0];
			y += dir_front[dir][1];
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
		else
		{
			x += dir_frontright[dir][0];
			y += dir_frontright[dir][1];
			dir = (dir + 1) % 4;
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
	}
	*num = step;
}

// 右手Mat迷宫巡线
void FindLine_RighThand_Mat(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num)
{
	//assert(img && img->data);
	assert(num && *num >= 0);
	assert(block_size > 1 && block_size % 2 == 1);
	int half = block_size / 2;
	int step = 0, dir = 0, turn = 0;
	while (step < *num && 0 < x && x < img.cols - 1 && 0 < y && y < img.rows - 1 && turn < 4)
	{
		int local_thres = 0;
		for (int dy = -half; dy <= half; dy++)
		{
			for (int dx = -half; dx <= half; dx++)
			{
				local_thres += img.at<uchar>(y + dy, x + dx);
			}
		}
		local_thres /= block_size * block_size;
		local_thres -= clip_value;

		int current_value = img.at<uchar>(y, x);
		int front_value = img.at<uchar>(y + dir_front[dir][1], x + dir_front[dir][0]);
		int frontright_value = img.at<uchar>(y + dir_frontright[dir][1], x + dir_frontright[dir][0]);
		// int current_value = AT(img, x, y);
		// int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
		// int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
		if (front_value < local_thres)
		{
			dir = (dir + 3) % 4;
			turn++;
		}
		else if (frontright_value < local_thres)
		{
			x += dir_front[dir][0];
			y += dir_front[dir][1];
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
		else
		{
			x += dir_frontright[dir][0];
			y += dir_frontright[dir][1];
			dir = (dir + 1) % 4;
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
	}
	*num = step;
}

void FindLine_RighThand_Bluecloth(Mat& img, int block_size, int clip_value, int x, int y, int pts[][2], int* num)
{
	//assert(img && img->data);
	assert(num && *num >= 0);
	assert(block_size > 1 && block_size % 2 == 1);
	int half = block_size / 2;
	int step = 0, dir = 0, turn = 0;
	while (step < *num && 0 < x && x < img.cols - 1 && 0 < y && y < img.rows - 1 && turn < 4)
	{
		int local_thres = 0;
		for (int dy = -half; dy <= half; dy++)
		{
			for (int dx = -half; dx <= half; dx++)
			{
				local_thres += img.at<uchar>(y + dy, x + dx);
			}
		}
		local_thres /= block_size * block_size;
		local_thres -= clip_value;

		int current_value = img.at<uchar>(y, x);
		int front_value = img.at<uchar>(y + dir_front[dir][1], x + dir_front[dir][0]);
		int frontright_value = img.at<uchar>(y + dir_frontright[dir][1], x + dir_frontright[dir][0]);
		// int current_value = AT(img, x, y);
		// int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
		// int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
		if (front_value > local_thres)
		{
			dir = (dir + 3) % 4;
			turn++;
		}
		else if (frontright_value > local_thres)
		{
			x += dir_front[dir][0];
			y += dir_front[dir][1];
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
		else
		{
			x += dir_frontright[dir][0];
			y += dir_frontright[dir][1];
			dir = (dir + 1) % 4;
			pts[step][0] = x;
			pts[step][1] = y;
			step++;
			turn = 0;
		}
	}
	*num = step;
}
// 折线段近似
void Approx_Lines(int pts[][2], int pts_num, float epsilon, int lines[][2], int* lines_num)
{
	assert(pts);
	assert(epsilon > 0);

	int dx = pts[pts_num - 1][0] - pts[0][0];
	int dy = pts[pts_num - 1][1] - pts[0][1];
	float nx = -dy / sqrtf(dx * dx + dy * dy);
	float ny = dx / sqrtf(dx * dx + dy * dy);
	float max_dist = 0, dist;
	int idx = -1;
	for (int i = 1; i < pts_num - 1; i++) {
		dist = fabs((pts[i][0] - pts[0][0]) * nx + (pts[i][1] - pts[0][1]) * ny);
		if (dist > max_dist) {
			max_dist = dist;
			idx = i;
		}
	}
	if (max_dist >= epsilon) {
		int num1 = *lines_num;
		Approx_Lines(pts, idx + 1, epsilon, lines, &num1);
		int num2 = *lines_num - num1 - 1;
		Approx_Lines(pts + idx, pts_num - idx, epsilon, lines + num1 - 1, &num2);
		*lines_num = num1 + num2 - 1;
	}
	else {
		lines[0][0] = pts[0][0];
		lines[0][1] = pts[0][1];
		lines[1][0] = pts[pts_num - 1][0];
		lines[1][1] = pts[pts_num - 1][1];
		*lines_num = 2;
	}
}

// float类型的折线段近似
void Approx_Lines_f(float pts[][2], int pts_num, float epsilon, float lines[][2], int* lines_num)
{
	assert(pts);
	assert(epsilon > 0);

	int dx = pts[pts_num - 1][0] - pts[0][0];
	int dy = pts[pts_num - 1][1] - pts[0][1];
	float nx = -dy / sqrtf(dx * dx + dy * dy);
	float ny = dx / sqrtf(dx * dx + dy * dy);
	float max_dist = 0, dist;
	int idx = -1;
	for (int i = 1; i < pts_num - 1; i++) {
		dist = fabs((pts[i][0] - pts[0][0]) * nx + (pts[i][1] - pts[0][1]) * ny);
		if (dist > max_dist) {
			max_dist = dist;
			idx = i;
		}
	}
	if (max_dist >= epsilon && *lines_num > 2) {
		int num1 = *lines_num;
		Approx_Lines_f(pts, idx + 1, epsilon, lines, &num1);
		int num2 = *lines_num - num1 - 1;
		Approx_Lines_f(pts + idx, pts_num - idx, epsilon, lines + num1 - 1, &num2);
		*lines_num = num1 + num2 - 1;
	}
	else {
		lines[0][0] = pts[0][0];
		lines[0][1] = pts[0][1];
		lines[1][0] = pts[pts_num - 1][0];
		lines[1][1] = pts[pts_num - 1][1];
		*lines_num = 2;
	}
}

// 计算大津阈值
uint16_t GetOSTUThreshold(image_t* img, uint8_t MinThreshold, uint8_t MaxThreshold)
{
	uint8_t Histogram[256];
	uint16_t OUSTThreshold = 0;
	uint32_t PixelAmount = 0, Value_Sum = 0;
	uint64_t sigma = 0, maxSigma = 0;
	float w1 = 0, w2 = 0;
	int32_t u1 = 0, u2 = 0;
	uint8_t MinValue = 0, MaxValue = 255;

	//各像素点个数
	uint8_t* ptr = img->data;
	uint8_t* ptrEnd = img->data + (img->width) * (img->height);
	while (ptr != ptrEnd)
	{
		++Histogram[*ptr++];
	}

	for (uint8_t m = 0; m < 100; m++) {

		Histogram[m] = 0;
	}

	for (MinValue = 0; Histogram[MinValue] == 0 && MinValue < 255; ++MinValue);
	for (MaxValue = 255; Histogram[MaxValue] == 0 && MaxValue > 0; --MaxValue);

	if (MaxValue == MinValue) return MaxValue;         // 只有一个颜色
	if (MinValue + 1 == MaxValue) return MinValue;        // 只有二个颜色

	if (MinValue < MinThreshold) {
		MinValue = MinThreshold;
	}
	if (MaxValue > MaxThreshold) {
		MaxValue = MaxThreshold;
	}

	uint32_t Pixel_Integral[256] = { 0 };   //像素积分 
	uint32_t Value_Integral[256] = { 0 };    //灰度积分
	for (uint8_t i = MinValue; i <= MaxValue; ++i) {
		PixelAmount += Histogram[i];      //像素总数
		Value_Sum += Histogram[i] * i;     //灰度总和
		Pixel_Integral[i] = PixelAmount;
		Value_Integral[i] = Value_Sum;
	}
	for (uint8_t i = MinValue; i < MaxValue + 1; ++i) {
		w1 = (float)Pixel_Integral[i] / PixelAmount;  //前景像素点比例
		w2 = 1 - w1;                               //背景比例
		u1 = (int32_t)(Value_Integral[i] / w1);                   //前景平均灰度
		u2 = (int32_t)((Value_Sum - Value_Integral[i]) / w2);      //背景平均灰度
		sigma = (uint64_t)((w1) * (w2) * (u1 - u2) * (u1 - u2));
		if (sigma >= maxSigma) {
			maxSigma = sigma;
			OUSTThreshold = i;
		}
		else {
			break;
		}
	}
	return OUSTThreshold;
}

//优化版大津法
#define GrayScale 256
uint8 pixel[GrayScale] = { 0 };

uint8 MyOSTU(Mat& img1, int star_y, int width)//起始行局部大津法
{
	int threshold = 0;
	int32_t sum_gray = 0;
	int32_t sum_pix_num = 0;
	int32_t pl_pix_num = 0;
	int32_t p2_pix_mum = 0;
	int32_t p1_sum_gray = 0;
	float m1 = 0;
	float m2 = 0;
	float V = 0;
	float variance = 0;
	int i, j, k = 0;
	unsigned char* ptr;
	//每一级灰度值每一帧图形都得清零
	// for (i = 0; i < 256; i++)
	// 	pixel[i] = 0;
	//统计起始行每个灰度级中像素的个数   
	for (int y = star_y; y > star_y - 3; y--)
	{
		for (j = 0; j < width; j++)
		{
			pixel[img1.at<uchar>(star_y, j)]++;
			// pixel[AT_IMAGE(img1, j, star_y)]++;
		}
	}


	for (k = 0; k < GrayScale; k++)
	{
		sum_gray += k * pixel[k];//灰度直方图质量矩
		sum_pix_num += pixel[k];//总像素个数
	}

	for (k = 0; k < GrayScale - 1; k++)
	{
		pl_pix_num += pixel[k];//第一部分像素个数
		p2_pix_mum = sum_pix_num - pl_pix_num;//第二部分像素个数
		p1_sum_gray += k * pixel[k];   //第一部分质量矩
		m1 = (float)p1_sum_gray / pl_pix_num;//第一部分灰度均值
		m2 = (float)(sum_gray - p1_sum_gray) / p2_pix_mum;//第二部分灰度均值

		V = pl_pix_num * p2_pix_mum * (m1 - m2) * (m1 - m2);

		if (V > variance)//将类间方差较大时的灰度值作为阈值
		{
			variance = V;
			threshold = k;
		}
	}
	return threshold;
}

// 点集三角滤波
void Blur_Points(float pts_in[][2], int num, float pts_out[][2], int kernel)
{
	assert(kernel % 2 == 1);
	int half = kernel / 2;
	for (int i = 0; i < num; i++)
	{
		pts_out[i][0] = pts_out[i][1] = 0;
		for (int j = -half; j <= half; j++)
		{
			pts_out[i][0] += pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
			pts_out[i][1] += pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
		}
		pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
		pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
	}
}

// 点集等距采样  使走过的采样前折线段的距离为`dist`
void Resample_Points(float pts_in[][2], int num1, float pts_out[][2], int* num2, float dist)
{
	int remain = 0, len = 0;
	for (int i = 0; i < num1 - 1 && len < *num2; i++)
	{
		float x0 = pts_in[i][0];
		float y0 = pts_in[i][1];
		float dx = pts_in[i + 1][0] - x0;
		float dy = pts_in[i + 1][1] - y0;
		float dn = sqrt(dx * dx + dy * dy);
		dx /= dn;
		dy /= dn;

		while (remain < dn && len < *num2)
		{
			x0 += dx * remain;
			pts_out[len][0] = x0;
			y0 += dy * remain;
			pts_out[len][1] = y0;

			len++;
			dn -= remain;
			remain = dist;
		}
		remain -= dn;
	}
	*num2 = len;
}

// 点集等距采样2  使采样后点与点的距离为`dist`
// TODO: fix bug
void Resample_Points2(float pts_in[][2], int num1, float pts_out[][2], int* num2, float dist)
{
	if (num1 < 0) {
		*num2 = 0;
		return;
	}
	pts_out[0][0] = pts_in[0][0];
	pts_out[0][1] = pts_in[0][1];
	int len = 1;
	for (int i = 0; i < num1 - 1 && len < *num2; i++) {
		float x0 = pts_in[i][0];
		float y0 = pts_in[i][1];
		float x1 = pts_in[i + 1][0];
		float y1 = pts_in[i + 1][1];

		do {
			float x = pts_out[len - 1][0];
			float y = pts_out[len - 1][1];

			float dx0 = x0 - x;
			float dy0 = y0 - y;
			float dx1 = x1 - x;
			float dy1 = y1 - y;

			float dist0 = sqrt(dx0 * dx0 + dy0 * dy0);
			float dist1 = sqrt(dx1 * dx1 + dy1 * dy1);

			float r0 = (dist1 - dist) / (dist1 - dist0);
			float r1 = 1 - r0;

			if (r0 < 0 || r1 < 0) break;
			x0 = x0 * r0 + x1 * r1;
			y0 = y0 * r0 + y1 * r1;
			pts_out[len][0] = x0;
			pts_out[len][1] = y0;
			len++;
		} while (len < *num2);

	}
	*num2 = len;
}

// 点集局部角度变化率
void Local_Angle_Points(float pts_in[][2], int num, float angle_out[], int dist)
{
	for (int i = 0; i < num; i++) {
		if (i <= 0 || i >= num - 1) {
			angle_out[i] = 0;
			continue;
		}
		float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
		float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
		float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
		float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
		float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
		float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
		float c1 = dx1 / dn1;
		float s1 = dy1 / dn1;
		float c2 = dx2 / dn2;
		float s2 = dy2 / dn2;
		angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
	}
}

// 角度变化率非极大抑制
void Nms_Angle(float angle_in[], int num, float angle_out[], int kernel)
{
	kernel % 2 == 1;
	int half = kernel / 2;
	for (int i = 0; i < num; i++) {
		angle_out[i] = angle_in[i];
		for (int j = -half; j <= half; j++) {
			if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
				angle_out[i] = 0;
				break;
			}
		}
	}
}

// 左边线跟踪中线，经过数据处理的左边线，减去一半的赛道宽度就是左边线对应的中线
void Track_LeftLine(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist)
{
	for (int i = 0; i < num; i++)
	{
		float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
		float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
		float dn = sqrt(dx * dx + dy * dy);
		dx /= dn;
		dy /= dn;
		pts_out[i][0] = pts_in[i][0] - dy * dist;
		pts_out[i][1] = pts_in[i][1] + dx * dist;
	}
}

// 右边线跟踪中线，经过数据处理的右边线，减去一半的赛道宽度就是右边线对应的中线
void Track_RightLine(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist)
{
	for (int i = 0; i < num; i++)
	{
		float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
		float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
		float dn = sqrt(dx * dx + dy * dy);
		dx /= dn;
		dy /= dn;
		pts_out[i][0] = pts_in[i][0] + dy * dist;
		pts_out[i][1] = pts_in[i][1] - dx * dist;
	}
}
float thres = 125;//起始行阈值
float block_size = 7;//根据分辨率大小调整
float clip_value = 2;
float begin_x = 43;//边线爬取的起始X，和跟踪起始点不太一样
float begin_y = 150;//边线爬取的起始Y，和跟踪起始点不太一样
float line_blur_kernel = 7;
float pixel_per_meter = 98;//矫正透视透视后，1m=nPixel；不同的摄像头,这个值不一样。
float sample_dist = 0.02;//采样距离
float angle_dist = 0.20;//角变化率的距离(只能在0.2左右微调，不然要角点会判断得不准)
float angle = 0.0;//舵机打角
float far_rate = 0.5;
float aim_distance = 0.78;//预瞄点的位置
void Process_GaryImage() // 边线提取&处理
{
	// 原图找左右边线,需要优化的就是起始点的查找	
	thres = MyOSTU(image_src, begin_y, Images_Cols);
	//cout << "thres = " << thres << endl;
	int x1 = image_src.cols / 2 - begin_x + 2, y1 = begin_y;
	ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);
	for (; x1 > 0; x1--) if (image_src.at<uchar>(y1, x1 - 1) < thres) break;
	if (image_src.at<uchar>(y1, x1) >= thres)
		FindLine_LefThand_Mat(image_src, block_size, clip_value, x1, y1, ipts0, &ipts0_num);
	else ipts0_num = 0;
	int x2 = image_src.cols / 2 + begin_x - 2, y2 = begin_y;
	ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
	for (; x2 < image_src.cols - 1; x2++) if (image_src.at<uchar>(y2, x2 + 1) < thres) break;
	if (image_src.at<uchar>(y2, x2) >= thres)
		FindLine_RighThand_Mat(image_src, block_size, clip_value, x2, y2, ipts1, &ipts1_num);
	else ipts1_num = 0;

	//去畸变+透视变换
	for (int i = 0; i < ipts0_num; i++)
	{
		rpts0[i][0] = mapx[ipts0[i][1]][ipts0[i][0]];
		rpts0[i][1] = mapy[ipts0[i][1]][ipts0[i][0]];
	}
	rpts0_num = ipts0_num;
	for (int i = 0; i < ipts1_num; i++)
	{
		rpts1[i][0] = mapx[ipts1[i][1]][ipts1[i][0]];
		rpts1[i][1] = mapy[ipts1[i][1]][ipts1[i][0]];
	}
	rpts1_num = ipts1_num;
	//边线滤波和边线的等距采样都是对爬出来的边线进行前期的数据处理
	// 边线滤波，点集三角滤波,边线点数数量传递
	Blur_Points(rpts0, rpts0_num, rpts0b, (int)round(line_blur_kernel));
	rpts0b_num = rpts0_num;
	Blur_Points(rpts1, rpts1_num, rpts1b, (int)round(line_blur_kernel));
	rpts1b_num = rpts1_num;

	// 边线等距采样，2cm采集一个点 需要利用赛道宽度找出 像素点跟实际距离的比例
	rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]);
	Resample_Points2(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter);//sample_dist * pixel_per_meter 把边线等距采样距离换算成像素点
	rpts1s_num = sizeof(rpts1s) / sizeof(rpts1s[0]);
	Resample_Points2(rpts1b, rpts1b_num, rpts1s, &rpts1s_num, sample_dist * pixel_per_meter);
	//边线局部角度变化率
	//边线局部角度变化率,经过角度变化率非极大抑制提供给后面元素角度判断使用
	Local_Angle_Points(rpts0s, rpts0s_num, rpts0a, (int)round(angle_dist / sample_dist));//angle_dist / sample_dist 
	rpts0a_num = rpts0s_num;
	Local_Angle_Points(rpts1s, rpts1s_num, rpts1a, (int)round(angle_dist / sample_dist));
	rpts1a_num = rpts1s_num;

	// 角度变化率非极大抑制
	Nms_Angle(rpts0a, rpts0a_num, rpts0an, (int)round(angle_dist / sample_dist) * 2 + 1);
	rpts0an_num = rpts0a_num;
	Nms_Angle(rpts1a, rpts1a_num, rpts1an, (int)round(angle_dist / sample_dist) * 2 + 1);
	rpts1an_num = rpts1a_num;

	// 左右边线（边线等距采样后的）对应的中线跟踪，两条中线，根据不同情况循迹
	Track_LeftLine(rpts0s, rpts0s_num, rptsc0, (int)round(angle_dist / sample_dist), pixel_per_meter * (ROAD_WIDTH / 2 ));
	rptsc0_num = rpts0s_num;//利用左边线得出中线
	Track_RightLine(rpts1s, rpts1s_num, rptsc1, (int)round(angle_dist / sample_dist), pixel_per_meter * (ROAD_WIDTH / 2 ));
	rptsc1_num = rpts1s_num;//利用右边线得出中线
}

void Process_Corners()
{
	// 识别Y,L拐点 每一帧图片都要清理
	Ypt0_rpts0s_id = Lpt0_rpts0s_id = Ypt1_rpts1s_id = Lpt1_rpts1s_id = 0;
	Ypt0_found = Ypt1_found = Lpt0_found = Lpt1_found = false;//Y,L两个角点是否有识别到标志位
	//判断左右边线是否大于一米，大于1.0米再继续判断是否为长直道√
	is_straight0 = rpts0s_num > 1.0 / sample_dist;
	is_straight1 = rpts1s_num > 1.0 / sample_dist;
	is_longstraight0 = rpts0s_num > 1.5 / sample_dist;
	is_longstraight1 = rpts1s_num > 1.0 / sample_dist;
	for (int i = 0; i < rpts0s_num; i++)//左边线判断
	{
		if (rpts0an[i] == 0) continue;//跳过判断这个点
		//角点计算的上下20cm两个点限幅
		int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, rpts0s_num - 1);
		int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, rpts0s_num - 1);
		//conf:
		float conf = fabs(rpts0a[i]) - (fabs(rpts0a[im1]) + fabs(rpts0a[ip1])) / 2;//换算成弧度制

		//Y角点阈值  -- 判断0.8米以内的第一个Y角点
		if (Ypt0_found == false && 40. / 180. * PI < conf && conf < 65. / 180. * PI && i < 0.95 / sample_dist)
		{
			Ypt0_rpts0s_id = i;
			Ypt0_found = true;
		}
		//L角点阈值  -- 判断0.8米以内的第一个L角点
		if (Lpt0_found == false && 70. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.95 / sample_dist)
		{
			Lpt0_rpts0s_id = i;
			Lpt0_found = true;
		}
		//长直道阈值
		if (conf > 5. / 180. * PI && i < 1.0 / sample_dist) is_straight0 = false;
		if (conf > 5. / 180. * PI && i < 2.0 / sample_dist) is_longstraight0 = false;
		if (Ypt0_found == true && Lpt0_found == true && is_straight0 == false) break;//0.8m以内同时判到Y、L角点，并且不是直线退出,进入二次判断
		
		
	}
	for (int i = 0; i < rpts1s_num; i++)//右边线判断
	{
		if (rpts1an[i] == 0) continue;
		int im1 = clip(i - (int)round(angle_dist / sample_dist), 0, rpts1s_num - 1);
		int ip1 = clip(i + (int)round(angle_dist / sample_dist), 0, rpts1s_num - 1);
		float conf = fabs(rpts1a[i]) - (fabs(rpts1a[im1]) + fabs(rpts1a[ip1])) / 2;
		if (Ypt1_found == false && 40. / 180. * PI < conf && conf < 65. / 180. * PI && i < 0.95 / sample_dist)
		{
			Ypt1_rpts1s_id = i;
			Ypt1_found = true;//初步认为是Y角点，如果没有被后面否定，他就是角点
		}
		if (Lpt1_found == false && 70. / 180. * PI < conf && conf < 140. / 180. * PI && i < 0.95 / sample_dist)
		{
			Lpt1_rpts1s_id = i;
			Lpt1_found = true;//初步认为是L角点,如果没有被后面否定，他就是角点
		}

		if (conf > 5. / 180. * PI && i < 1.0 / sample_dist) is_straight1 = false;
		if (conf > 5. / 180. * PI && i < 2.0 / sample_dist) is_longstraight1 = false;

		if (Ypt1_found == true && Lpt1_found == true && is_straight1 == false) break;
	}
	// Y点二次检查,依据两角点距离及角点后张开特性
	if (Ypt0_found && Ypt1_found)//左右两边都检查到Y角点
	{
		float dx = rpts0s[Ypt0_rpts0s_id][0] - rpts1s[Ypt1_rpts1s_id][0];
		float dy = rpts0s[Ypt0_rpts0s_id][1] - rpts1s[Ypt1_rpts1s_id][1];
		float dn = sqrtf(dx * dx + dy * dy);//计算两Y角点的距离，像素点距离
		if (fabs(dn - 0.45 * pixel_per_meter) < 0.15 * pixel_per_meter)
		{
			float dwx = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
				rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
			float dwy = rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
				rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
			float dwn = sqrtf(dwx * dwx + dwy * dwy);//计算两个Y角点伸展1米处的距离，这个算出来时像素点的距离
			if (!(dy < 0.3 * pixel_per_meter) ||
				!(dwn > 0.7 * pixel_per_meter &&
					rpts0s[clip(Ypt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Ypt0_rpts0s_id][0] &&
					rpts1s[clip(Ypt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Ypt1_rpts1s_id][0]))//不满足角点张开特性，否定
			{
				Ypt0_found = Ypt1_found = false;
			}
		}
		else //两角点相距太大，否定
		{
			Ypt0_found = Ypt1_found = false;
		}
	}
	// L点二次检查，车库模式不检查, 依据L角点距离及角点后张开特性
	if (Garage_Type == GARAGE_NONE)
	{
		if (Lpt0_found && Lpt1_found)//左右两边同时判断到角点
		{
			float dx = rpts0s[Lpt0_rpts0s_id][0] - rpts1s[Lpt1_rpts1s_id][0];
			float dy = rpts0s[Lpt0_rpts0s_id][1] - rpts1s[Lpt1_rpts1s_id][1];
			float dn = sqrtf(dx * dx + dy * dy);//计算两角点的位置
			if (fabs(dn - 0.40 * pixel_per_meter) < 0.15 * pixel_per_meter)
			{
				float dwx = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] -
					rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0];
				float dwy = rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][1] -
					rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][1];
				float dwn = sqrtf(dwx * dwx + dwy * dwy);//计算两个L角点伸展1米处的距离
				if (!(dwn > 0.7 * pixel_per_meter &&
					rpts0s[clip(Lpt0_rpts0s_id + 50, 0, rpts0s_num - 1)][0] < rpts0s[Lpt0_rpts0s_id][0] &&
					rpts1s[clip(Lpt1_rpts1s_id + 50, 0, rpts1s_num - 1)][0] > rpts1s[Lpt1_rpts1s_id][0]))
				{
					Lpt0_found = Lpt1_found = false;
				}
			}
			else
			{
				Lpt0_found = Lpt1_found = false;
			}
		}
	}
}
//(纯跟踪起始点) 给车定义一个起点坐标
float cx = mapx[(int)(Images_Rows * 0.84f)][(Images_Cols/2)+10];
float cy = mapy[(int)(Images_Rows * 0.84f)][(Images_Cols/2)+10];
int servo = 0, motor = 0;
int aim_idx = 0, aim_idx_near = 0;//远近预锚点的位置;
void Tradition_Follows()
{
	// 等待opencv采集usb摄像头完毕,
	aim_distance = 0.65;//单位cm
	Process_GaryImage();    // 边线提取&处理,图像处理,正常赛道
	Process_Corners();     // 角点提取&筛选，特征点提取用来判断元素
	//单侧线少，切换巡线方向  切外向圆(这个细节注意一下)这个也是精华，路径的好坏跟这个也有很大的关系
	if (rpts0s_num < rpts1s_num / 1.5 && rpts0s_num < 60) //如果左边线少于右边线一半，并且左边线数小于60
	{
		Track_Type = TRACK_RIGHT;//切换为寻右边线模式
	}
	else if (rpts1s_num < rpts0s_num / 1.5 && rpts1s_num < 60) //如果不满足上述条件，且如果右边线少于左边线一半，并且右边线数小于60
	{
		Track_Type = TRACK_LEFT;//切换为寻左边线模式
	}
	else if (rpts0s_num < 30 && rpts1s_num > rpts0s_num) //如果不满足上述条件，左边线数小于20，并且右边线大于左边线
	{
		Track_Type = TRACK_RIGHT;//切换为寻右边线模式
	}
	else if (rpts1s_num < 30 && rpts0s_num > rpts1s_num)
	{
		Track_Type = TRACK_LEFT;//切换为寻左边线模式
	}
	//Track_Type = TRACK_LEFT;//切换为寻左边线模式
	//↑上面是正常循迹
	//↓下面是对元素的处理
	//车库斑马线检查(斑马线优先级高，最先检查) 打算使用目标检测
	// if(!(WorkZone_type == WORKZONE_LEFT_IN||WorkZone_type == WORKZONE_LEFT_OUT))
	Check_Garage();
	// USB2.0灰度摄像头检查Tracklogo(找赛道上的黑斑)
	// if (Garage_Type == GARAGE_NONE && Cross_Type == CROSS_NONE && Yroad_Type == YROAD_NONE)
	// 	check_tracklogo();
	// 分别检查十字 三叉 和圆环, 十字优先级最高
	if (Garage_Type == GARAGE_NONE)
		Check_Cross();//十字检查,检查L角点
	if (Garage_Type == GARAGE_NONE && Cross_Type == CROSS_NONE && Circle_Type == CIRCLE_NONE)
		Check_Yroad();//三叉检查
	// if (Garage_Type == GARAGE_NONE && Cross_Type == CROSS_NONE && Yroad_Type == YROAD_NONE)
	// 	Check_Circle();//圆环检查
	if (Cross_Type != CROSS_NONE)
	{
		Circle_Type = CIRCLE_NONE;
		Yroad_Type = YROAD_NONE;
		WorkZone_Type = WORKZONE_NONE;
	}
	if (Garage_Type != GARAGE_NONE)
	{
		Circle_Type = CIRCLE_NONE;
	}
	if (Garage_Type != GARAGE_NONE || Cross_Type != CROSS_NONE || Yroad_Type != YROAD_NONE || Circle_Type != CIRCLE_NONE)
	{
		WorkZone_Type = WORKZONE_NONE;
		GasStation_Type = GASSTATION_NONE;
	}
	//根据检查结果执行模式
	if (Garage_Type != GARAGE_NONE) Run_Garage();
	else if (Cross_Type != CROSS_NONE) Run_Cross();
	else if (Yroad_Type != YROAD_NONE) Run_Yroad();
	else if (Circle_Type != CIRCLE_NONE) Run_Circle();
	//if (tracklogo_type == TRACKLOGO_FOUND || tracklogo_type == TRACKLOGO_MAYBE) aim_distance = 35;
	// 中线跟踪,根据单侧线多线少切换模式
	if (Cross_Type == CROSS_IN)
	{
		if (Track_Type == TRACK_LEFT)
		{
			rpts = rptsc0;//预先分配内存
			Track_LeftLine(far_rpts0s + far_Lpt0_rpts0s_id, far_rpts0s_num - far_Lpt0_rpts0s_id, rpts,
				(int)round(angle_dist / sample_dist), pixel_per_meter * (ROAD_WIDTH / 2 ));
			rpts_num = far_rpts0s_num - far_Lpt0_rpts0s_id;
		}
		else
		{
			rpts = rptsc1;
			Track_RightLine(far_rpts1s + far_Lpt1_rpts1s_id, far_rpts1s_num - far_Lpt1_rpts1s_id, rpts,
				(int)round(angle_dist / sample_dist), pixel_per_meter * (ROAD_WIDTH / 2));
			rpts_num = far_rpts1s_num - far_Lpt1_rpts1s_id;
		}
	}
	else if (Yroad_Type == YROAD_NEAR)
	{
		rpts = rptsc1;
		Track_RightLine(faryroad_rpts1s, faryroad_rpts1s_num, rpts, (int)round(angle_dist / sample_dist), pixel_per_meter * (ROAD_WIDTH / 2 ));
		rpts_num = faryroad_rpts1s_num;//利用右边线得出中线
	}
	else if (Yroad_Type == GENERIC_IN)
	{
		rpts = rptsc1;
		Track_RightLine(generic_rpts1s, generic_rpts1s_num, rpts, (int)round(angle_dist / sample_dist), pixel_per_meter * (ROAD_WIDTH / 2 ));
		rpts_num = generic_rpts1s_num;//利用右边线得出中线
	}
	else if (Yroad_Type == GENERIC_OUT)
	{
		Track_LeftLine(generic_rpts0s, generic_rpts0s_num, rpts, (int)round(angle_dist / sample_dist), pixel_per_meter * (ROAD_WIDTH / 2 ));
		rpts_num = generic_rpts0s_num;//利用右边线得出中线

	}
	else
	{
		if (Track_Type == TRACK_LEFT)
		{
			rpts = rptsc0;
			rpts_num = rptsc0_num;
		}
		else
		{
			rpts = rptsc1;
			rpts_num = rptsc1_num;
		}
	}
	// 找最近点(起始点中线归一化)
	float min_dist = 1e10;
	int begin_id = -1;
	// if (Track_Type == TRACK_RIGHT)
	// {
	// 	cx = mapx[(int)(Images_Rows * 0.84f)][(Images_Cols/2)+16];
	// 	cy = mapy[(int)(Images_Rows * 0.84f)][(Images_Cols/2)+16];
	// }
	// else if(Track_Type == TRACK_LEFT)
	// {
	// 	cx = mapx[(int)(Images_Rows * 0.84f)][(Images_Cols/2)+25];
	// 	cy = mapy[(int)(Images_Rows * 0.84f)][(Images_Cols/2)+25];
	// }
	for (int i = 0; i < rpts_num; i++)
	{
		float dx = rpts[i][0] - cx;
		float dy = rpts[i][1] - cy;
		float dist = sqrt(dx * dx + dy * dy);
		if (dist < min_dist)
		{
			min_dist = dist;
			begin_id = i;
		}
	}

	// 特殊模式下，不找最近点(由于边线会绕一圈回来，导致最近点为边线最后一个点，从而中线无法正常生成)
	if (Garage_Type == GARAGE_IN_LEFT || Garage_Type == GARAGE_IN_RIGHT || Cross_Type == CROSS_IN) begin_id = 0;
	// 中线有点，同时最近点不是最后几个点
	if (begin_id >= 0 && rpts_num - begin_id >= 3)
	{
		// 归一化中线
		rpts[begin_id][0] = cx;
		rpts[begin_id][1] = cy;
		rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);
		Resample_Points2(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);//归一化中线等距采样
		// 远预锚点位置
		aim_idx = clip(round(aim_distance / sample_dist), 0, rptsn_num - 1);
		// 近预锚点位置
		aim_idx_near = clip(round(0.30 / sample_dist), 0, rptsn_num - 1);
		// 计算远锚点偏差值
		float dx = rptsn[aim_idx][0] - cx;
		float dy = cy - rptsn[aim_idx][1];
		float dn = sqrt(dx * dx + dy * dy);
		float error = -atan2f(dx, dy) * 180 / PI;
		//assert(!isnan(error));
		// 若考虑近点远点,可近似构造Stanley算法,避免撞路肩
		// 计算近锚点偏差值
		float dx_near = rptsn[aim_idx_near][0] - cx;
		float dy_near = cy - rptsn[aim_idx_near][1];
		float dn_near = sqrt(dx_near * dx_near + dy_near * dy_near);
		float error_near = -atan2f(dx_near, dy_near) * 180 / PI;
		//assert(!isnan(error_near));
		// 远近锚点综合考虑
		//angle = pid_solve(&servo_pid, error * far_rate + error_near * (1 - far_rate));
		// 根据偏差进行PD计算
		//float angle = pid_solve(&servo_pid, error);
		// 纯跟踪算法(只考虑远点)
		float pure_angle = -atanf(pixel_per_meter * 2 * 0.2 * dx / dn / dn) / PI * 180 / SMOTOR_RATE;
		//cout<<"pure_angle="<<pure_angle<<endl;
		// if(is_longstraight0 && is_longstraight0)
		// {
		// 	PID_Servo.P=0.55;
		// }
		// else
		// {
		// 	PID_Servo.P=0.66;
		// }
		angle = Servo_PID(&PID_Servo, pure_angle);
		//cout<<"angle="<<angle<<endl;
		servo = angle * 100;
		if (Garage_Type == GARAGE_BEGINOUT_LEFT)
		{
			servo = 0;
		}
		else if (Garage_Type == GARAGE_OUT_LEFT)
		{
			servo = angle * 185;
		}
		if (Yroad_Type == GENERIC_OUT)
		{
			servo = angle * 130;//出三岔路口处理
		}
	}

}