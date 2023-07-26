#pragma once
#ifndef SMOTOR_H_H
#define SMOTOR_H_H

#define SERVO_FREQ      (50)

// 前轮转角和方向舵机转角的比例关系
#define SMOTOR_RATE     (1)

void speed_control(void);

#endif
