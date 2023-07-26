#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"
using namespace paddle::lite_api;
using namespace cv;
using namespace std;
std::shared_ptr<paddle::lite_api::PaddlePredictor> g_predictor;
float* Detection_Output_Data;
int Detection_Output_Size;


const char* label_name[5] = { "WorkZone",
							 "GasStation",
							 "Ramp",
							"WriteOne",
							"WriteTwo",
};

void DeeplearningDetectionInitial()
{
	std::vector<Place> valid_places({
		Place{TARGET(kFPGA), PRECISION(kFP16), DATALAYOUT(kNHWC)},
		Place{TARGET(kHost), PRECISION(kFloat)},
		Place{TARGET(kARM), PRECISION(kFloat)},
		});
	paddle::lite_api::CxxConfig config;
	config.set_valid_places(valid_places);
	config.set_model_file("../vision/Neural_networks/Smartcar_Model/__model__");//模型结构
	config.set_param_file("../vision/Neural_networks//Smartcar_Model/__params__");//模型参数
	auto predictor = paddle::lite_api::CreatePaddlePredictor(config);//这个api很重要
	g_predictor = predictor;
}

void DeeplearningDetection(Mat& img_raw0)
{
	auto input = g_predictor->GetInput(0);//声明一块临时变量
	input->Resize({ 1, 3, 300, 300 });//
	auto* in_data = input->mutable_data<float>();
	Mat img2;
	resize(img_raw0, img2, Size(300, 300));//压缩图片
	Mat sample_float;
	img2.convertTo(sample_float, CV_32FC3);
	int index = 0;
	//归一化
	for (int row = 0; row < sample_float.rows; ++row)
	{
		float* ptr = (float*)sample_float.ptr(row);
		for (int col = 0; col < sample_float.cols; col++)
		{
			float* uc_pixel = ptr;
			float b = uc_pixel[0];
			float g = uc_pixel[1];
			float r = uc_pixel[2];
			in_data[index] = (b - 127.5) / 127.502231;
			in_data[index + 1] = (g - 127.5) / 127.502231;
			in_data[index + 2] = (r - 127.5) / 127.502231;
			ptr += 3;
			index += 3;
		}
	}
	// //模型推理  
	g_predictor->Run();
	auto output = g_predictor->GetOutput(0);
	Detection_Output_Data = output->mutable_data<float>();
	Detection_Output_Size = output->shape()[0];
	// cout << "Detection_Output_Data = " << Detection_Output_Data[0] << endl;
	// cout << "Detection_Output_Size = " << Detection_Output_Size << endl;

	for (int i = 0; i < Detection_Output_Size; i++)
	{
		int cls = Detection_Output_Data[0];//看看如果不像demo那样写是不是会慢
		float score = Detection_Output_Data[1];
		//detection_type = CLASS_BACKGROUND;
		if (score > 0.9)
		{
			int y = 0;
			y = static_cast<int>(Detection_Output_Data[5] * img_raw0.rows);
			if (y > (img_raw0.rows * 0.5))
			{
				switch (cls) //枚举变量进行赋值，方便控制
				{
					// case 1: detection_type = CLASS_CONSTRUCTION; break;
					// case 2: detection_type = CLASS_FORBID; break;
					// case 3: detection_type = CLASS_GENERIC; break;
					// case 4: detection_type = CLASS_RAMP; break;
					// default: break;
				}

			}


			// int width = x2 - x1;
			// int height = y2 - y1;
			// Dectec.DetCls[cls].box_num++;
			// Dectec.DetCls[cls].boxmid_x[Dectec.DetCls[cls].box_num] = (x1+x2)/2;
			// Dectec.DetCls[cls].boxmid_y[Dectec.DetCls[cls].box_num] = (y1+y2)/2;

		}
		// else
		// {
		//     detection_type = CLASS_BACKGROUND;
		// }
		Detection_Output_Data += 6;
	}


}
void DeeplearningClassInitial()
{
	std::vector<Place> valid_places({
		Place{TARGET(kFPGA), PRECISION(kFP16), DATALAYOUT(kNHWC)},
		Place{TARGET(kHost), PRECISION(kFloat)},
		Place{TARGET(kARM), PRECISION(kFloat)},
		});
	paddle::lite_api::CxxConfig config;
	config.set_valid_places(valid_places);
	config.set_model_file("/root/workspace/ShenHongYT_17th/src/Vision/Neural_Networks/ClassModel/model");//模型结构
	config.set_param_file("/root/workspace/ShenHongYT_17th/src/Vision/Neural_Networks//ClassModel/params");//模型参数
	auto predictor = paddle::lite_api::CreatePaddlePredictor(config);//这个api很重要
	g_predictor = predictor;
}

enum deeplearning_type_e DeepLearning_Type = NoResults;

// 0	BusyArea 施工区
// 1	GasStation 加油站
// 2	Ramp 坡道
// 3	WriteOne 1
// 4	WriteTwo 2

deeplearning TrackRedCla = DEF_DLC(0, 0, 0, 0, 0);

void DeeplearningClass(Mat& img_raw0,Rect rect)//传统视觉处理获得地标再丢给网络 输入层16x16，耗时3ms左右
{

	rect = rect + Point(int(rect.tl().x * ratio_X - rect.tl().x), (rect.tl().y * ratio_Y - rect.tl().y));//平移到原图
	rect = rect + Size(int(rect.width * ratio_X - rect.width), (rect.height * ratio_Y - rect.height));
	roi_image = img_raw0(Rect(rect.tl().x, rect.tl().y, rect.width, rect.height)); //把地标裁出来
    // roi_image = img_raw0(rect);
	auto input = g_predictor->GetInput(0);
	input->Resize({ 1, 3, 64, 64 });
	auto* in_data = input->mutable_data<float>();

	Mat img_resize;
	resize(roi_image, img_resize, cv::Size(64, 64));
	Mat sample_float;
	img_resize.convertTo(sample_float, CV_32FC3);

	int index = 0;
	for (int row = 0; row < sample_float.rows; ++row)
	{
		float* ptr = (float*)sample_float.ptr(row);
		for (int col = 0; col < sample_float.cols; col++)
		{
			float* uc_pixel = ptr;
			float b = uc_pixel[0];
			float g = uc_pixel[1];
			float r = uc_pixel[2];
			// in_data[index] = (b - 127.5) / 127.502231;
			// in_data[index + 1] = (g - 127.5) / 127.502231;
			// in_data[index + 2] = (r - 127.5) / 127.502231;

			// 可以考虑训练的时候不转RGB
			in_data[index] = (r / 255.0 - 0.485) / 0.229;
			in_data[index + 1] = (g / 255.0 - 0.456) / 0.224;
			in_data[index + 2] = (b / 255.0 - 0.406) / 0.225;
			ptr += 3;
			index += 3;
		}
	}

	g_predictor->Run();
	auto output = g_predictor->GetOutput(0);
	Detection_Output_Data = output->mutable_data<float>();
	Detection_Output_Size = output->shape()[1];
	int max_index = -1;
	float score = 0;
	for (int i = 0; i < Detection_Output_Size; i++)
	{
		float value = Detection_Output_Data[i];
		if (value > 0.90)
		{
			max_index = i;
			score = value;
			switch (max_index) //检测到类别图像场数计数
			{
			case 0: TrackRedCla.WriteOne_Num++; break;
			case 1: TrackRedCla.WriteTwo_Num++; break;
			default: break;
			}
		break;
			
		}
	}
	
		
	
	
	// if(TrackRedcla.BusyArea_num>20) deeplearning_type=BusyArea;
	// else if(TrackRedcla.GasStation_num>20) deeplearning_type=GasStation_num;
	// else if(TrackRedcla.Ramp_num>20) deeplearning_type=Ramp_num;
	// else if(TrackRedcla.WriteOne_num>20) deeplearning_type=WriteOne_num;
	// else if(TrackRedcla.WriteTwo_num>20) deeplearning_type=WriteTwo_num;
	// printf("TrackRedCla.WorkZone_num: %d\n", TrackRedCla.WorkZone_Num);
	// printf("TrackRedCla.GasStation_num: %d\n", TrackRedCla.GasStation_Num);
	//printf("TrackRedCla.Ramp_num: %d\n", TrackRedCla.Ramp_Num);
	// printf("TrackRedCla.WriteOne_num: %d\n", TrackRedCla.WriteOne_Num);
	// printf("TrackRedCla.WriteTwo_num: %d\n", TrackRedCla.WriteTwo_Num);
}


void DeeplearningDisplay(Mat& img_raw0_mat)
{
	for (int i = 0; i < Detection_Output_Size; i++)
	{
		int cls = Detection_Output_Data[0];//看看如果不像demo那样写是不是会慢
		float score = Detection_Output_Data[1];
		if (score > 0.9) //cls有时候为-1，看起始标签，起始标签为1则这里大于0就行
		{
			int x1 = 0;
			int y1 = 0;
			int x2 = 0;
			int y2 = 0;
			x1 = static_cast<int>(Detection_Output_Data[2] * img_raw0_mat.cols);
			y1 = static_cast<int>(Detection_Output_Data[3] * img_raw0_mat.rows);
			x2 = static_cast<int>(Detection_Output_Data[4] * img_raw0_mat.cols);
			y2 = static_cast<int>(Detection_Output_Data[5] * img_raw0_mat.rows);
			int width = x2 - x1;
			int height = y2 - y1;
			Point pt1(x1, y1);
			Point pt2(x2, y2);
			rectangle(img_raw0_mat, pt1, pt2, cv::Scalar(0, 255, 0), 2);
			putText(img_raw0_mat, label_name[cls], pt1, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0, 255), 2);
			// cout << "score = " << score << endl;
			// cout << "cls = " << cls << endl;
			//cv::putText(img_raw0_mat, score, pt1, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0 , 255), 2);

			//临时测试用
		//     if (((y1 + y2) / 2) > (CameraH_Set / 2) && (CameraW_Set/2 - BeginW) < ((x1 + x2) / 2) && ((x1 + x2) / 2) < (CameraW_Set/2 + BeginW))
		//     {
		//         while (TCRecData.IfRunCar)
		//         {
		//             UART_SendCommand(1500, 0);
		//             GetTCData(&TCRecData);//其实也可以用ifSuccess
		//         }
		//     }
		}
		Detection_Output_Data += 6;
	}
	imshow("erode_image", img_raw0_mat);
	//imshow("dst", dstImg);
	waitKey(1);
}