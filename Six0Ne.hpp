#pragma once
#ifndef Six0Ne_H_H
#define Six0Ne_H_H
#include <iostream>
#include <sys/time.h>
#include <bits/stdc++.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <cstdlib>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "paddle_api.h"
#include "src/Control/Pid.hpp"
#include "src/Control/SpeedDecisions.hpp"
#include "src/Serial/Serial.hpp"
#include "src/Vision/GrayImagproc/GrayImgproc.hpp"
#include "src/Vision/GrayImagproc/Camera_Param.hpp"
#include "src/Vision/GrayImagproc/Camera_Set.hpp"
#include "src/Vision/GrayImagproc/Cross.hpp"
#include "src/Vision/GrayImagproc/Circle.hpp"
#include "src/Vision/GrayImagproc/Yroad.hpp"
#include "src/Vision/GrayImagproc/Garage.hpp"
#include "src/Vision/GrayImagproc/Display.hpp"
#include "src/Vision/GrayImagproc/Common.hpp"
#include "src/Vision/RgbImagesProc/RgbImgProc.hpp"
#include "src/Vision/RgbImagesProc/Ramp.hpp"
#include "src/Vision/RgbImagesProc/WorkZoon.hpp"
#include "src/Vision/RgbImagesProc/GasStation.hpp"
#include "src/Vision/Neural_Networks/DeepLearning.hpp"
using namespace cv;
using namespace std;


#endif // Six0Ne_H_H