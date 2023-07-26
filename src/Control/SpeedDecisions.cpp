#include "/root/workspace/ShenHongYT_17th/Six0Ne.hpp"

//#define MINMAX(x, l, u) MIN(MAX(x, l), u)
#define MOTOR_PWM_DUTY_MAX    50000


// 0.4s
//pid_param_t motor_pid_l = PID_CREATE(8830, 14117/1e3, 0, 1, MOTOR_PWM_DUTY_MAX, MOTOR_PWM_DUTY_MAX, MOTOR_PWM_DUTY_MAX);
//pid_param_t motor_pid_r = PID_CREATE(8830, 14117/1e3, 0, 1, MOTOR_PWM_DUTY_MAX, MOTOR_PWM_DUTY_MAX, MOTOR_PWM_DUTY_MAX);
// 0.3s
//pid_param_t motor_pid_l = PID_CREATE(11864, 22367/1e3, 0, 1, MOTOR_PWM_DUTY_MAX, MOTOR_PWM_DUTY_MAX, MOTOR_PWM_DUTY_MAX);
//pid_param_t motor_pid_r = PID_CREATE(11864, 22367/1e3, 0, 1, MOTOR_PWM_DUTY_MAX, MOTOR_PWM_DUTY_MAX, MOTOR_PWM_DUTY_MAX);



// 基准速度
float NORMAL_SPEED = 102;
// 当前目标速度
float target_speed;

//三叉识别速度    
float YROAD_FOUND_SPEED = 3, YROAD_NEAR_SPEED = 112;
//圆环速度 + NORMAL_SPEED
float CIRCLE_MAX_SPEED = 0, CIRCLE_MIN_SPEED = -20;
//识别速度 + NORMAL_SPEED
float LOGO_MAX_SPEED = 0, LOGO_MIN_SPEED = -60;
//速度限+  NORMAL_SPEED
float NORMAL_MAX_SPEED = 0, NORMAL_MIN_SPEED = -25;


// 三点圆弧半径
float radius_3pts(float pt0[2], float pt1[2], float pt2[2]) {
	float a, b, c, d, e, f, r, x, y;
	a = 2 * (pt1[0] - pt0[0]);
	b = 2 * (pt1[1] - pt0[1]);
	c = pt1[0] * pt1[0] + pt1[1] * pt1[1] - pt0[0] * pt0[0] - pt0[1] * pt0[1];
	d = 2 * (pt2[0] - pt1[0]);
	e = 2 * (pt2[1] - pt1[1]);
	f = pt2[0] * pt2[0] + pt2[1] * pt2[1] - pt1[0] * pt1[0] - pt1[1] * pt1[1];
	x = (b * f - e * c) / (b * d - e * a);
	y = (d * c - a * f) / (b * d - e * a);
	r = sqrt((x - pt0[0]) * (x - pt0[0]) + (y - pt0[1]) * (y - pt0[1]));
	return r;
}



// 起步特殊PID(已弃用)

void speed_control(void) //速度决策
{
	if (Garage_Type == GARAGE_BEGINOUT_LEFT || Garage_Type == GARAGE_BEGINOUT_RIGHT)
	{
		// 出库缓启动模式，避免一下冲的太猛，冲出赛道
		motor = 30;
	}
	else if (Garage_Type == GARAGE_OUT_LEFT || Garage_Type == GARAGE_OUT_RIGHT)
	{
		// 出库缓启动模式，避免一下冲的太猛，冲出赛道
		motor = 30;
	}
	else if (Garage_Type == GARAGE_PASS_LEFT || Garage_Type == GARAGE_PASS_RIGHT)
	{
		// 出库缓启动模式，避免一下冲的太猛，冲出赛道
		motor = 100;
	}
	else if (Garage_Type == GARAGE_IN_RIGHT || Garage_Type == GARAGE_IN_LEFT)
	{
		// 入库中减速,明天入口不行就改一下这个
		motor = 90;
	}
	else if (Cross_Type == CROSS_BEGIN)
	{
		motor = 135; // 缓慢前进避免完全停车后，OpenArt却没识别到apriltag的情况
		// 位置环停车(弃用)
	}
	
	// else if(Yroad_Type == YROAD_FOUND)
	// {
	// 	motor = 80;
	// }
	else if (Yroad_Type == YROAD_NEAR) //进入泛行区速度，稍微踩一脚油门
	{
		//三叉near, 近乎停车
		motor = 95;
	}
	else if (Yroad_Type == GENERIC_IN) //进入泛行区速度，稍微踩一脚油门
	{
		//三叉near, 近乎停车
		motor = 122;
	}
	else if (Yroad_Type == GENERIC_OUT) //进入泛行区速度，稍微踩一脚油门
	{
		//三叉near, 近乎停车
		motor = 92;
	}
	else if (Circle_Type == CIRCLE_RIGHT_BEGIN ||Circle_Type == CIRCLE_LEFT_BEGIN)
	{
		// 圆环速度  左圆环max 16.2 -1.5
		// 圆环开始，斜坡函数减速
		motor = 92;
	}
	else if(GasStation_Type == GASSTATION_LEFT_FOUND)
	{
		motor = 90;
	}
	else if(WorkZone_Type == WORKZONE_LEFT_BEGIN)
	{
		motor = 90;
	}
	// if (circle_type == CIRCLE_RIGHT_RUNNING ||circle_type == CIRCLE_LEFT_RUNNING)
	// {
	// 	// 圆环速度  左圆环max 16.2 -1.5
	// 	// 圆环开始，斜坡函数减速
	// 	motor = 92;
	// }
	// else if (circle_type == CIRCLE_RIGHT_OUT ||circle_type == CIRCLE_LEFT_OUT)
	// {
	// 	// 圆环速度  左圆环max 16.2 -1.5
	// 	// 圆环开始，斜坡函数减速
	// 	motor = 92;
	// }
	// else if (Circle_Type == CIRCLE_LEFT_END || Circle_Type == CIRCLE_RIGHT_END)
	// {
	// 	// 出环加速
	//  // 
	// 	motor = 122;
	// }
	
	else if (rptsn_num > 25)
	{
		// 直道/弯道速度根据斜率调速度
		int id = MIN(100, rptsn_num - 1);
		float error = fabs((rptsn[id][0] - rptsn[0][0]) / (rptsn[id][1] - rptsn[0][1]));
		if (is_straight0 && is_straight1 && error < 0.15)
		{
			motor = 136;
		}
		else
		{ 
			float speed = -Servo_PID(&Target_Speed_PID, error);
			motor = MINMAX(NORMAL_SPEED + speed, NORMAL_SPEED + NORMAL_MIN_SPEED, NORMAL_SPEED + NORMAL_MAX_SPEED);
		}
		

		// 减速加入kd, 突入大弯
//        if(error >= 0.5) target_speed_pid.kd = 20;
//        else target_speed_pid.kd = 0;

		
	}
	// else if (rptsn_num < 5) 
	// {
	//     //点太少,不对劲直接慢速
	//     motor = NORMAL_SPEED + NORMAL_MIN_SPEED;
	// }
	// 急停(车库停车，或电感值太小即冲出赛道)
	if (Garage_Type == GARAGE_STOP)
	{
		servo = 0, motor = 0, rgb_servo = 0, rgb_motor = 0;
		Serial_Send(&servo, &motor); //智能车停止运动
		cout << ">>>>>>>成功入库，退出程序 <<<<<<<<" << endl;
		exit(0);

	}
}

