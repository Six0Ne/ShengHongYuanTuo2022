cmake_minimum_required(VERSION 3.5.2)
project(ShenHongYT_17th)
#---------------------------------------------------------------------
#       [ Include、Lib  : Define ] ==> []
#---------------------------------------------------------------------
#set(COMMON_LIB_DIR "${PROJECT_SOURCE_DIR}/lib/")

#set(COMMON_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")


#link_directories(${COMMON_LIB_DIR})
#include_directories( ${COMMON_INCLUDE_DIR})
#---------------------------------------------------------------------
#       [ Six0Ne] ==> [smartcar]
#---------------------------------------------------------------------
#寻找OpenCV包
find_package(OpenCV)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
find_package(PaddleLite)
include_directories(${PADDLELITE_INCLUDE_DIR})

message(STATUS "PaddleLite found (${PADDLELITE_LIBRARY})")
message(STATUS "OpenCV found (${OpenCV_CONFIG_PATH})，${OpenCV_LIBS}")

#生成应用程序
set(MAIN main.cpp) #与CMakerLists.txt 同一个根目录下
set(VISION
		src/Vision/GrayImagproc/GrayImgproc.cpp
		src/Vision/GrayImagproc/Camera_Param.cpp
		src/Vision/GrayImagproc/Camera_Set.cpp
		src/Vision/GrayImagproc/Circle.cpp
		src/Vision/GrayImagproc/Cross.cpp
		src/Vision/GrayImagproc/Display.cpp
		src/Vision/GrayImagproc/Garage.cpp
		src/Vision/GrayImagproc/Utils.cpp
		src/Vision/GrayImagproc/Yroad.cpp
		src/Vision/RgbImagesProc/RgbImgProc.cpp
		src/Vision/RgbImagesProc/Ramp.cpp
		src/Vision/RgbImagesProc/WorkZoon.cpp
		src/Vision/RgbImagesProc/GasStation.cpp
		src/Vision/Neural_Networks/DeepLearning.cpp		
	)
set(CONTROL 
		src/Control/Pid.cpp
		src/Control/SpeedDecisions.cpp
	)
set(SERIAL 
		src/Serial/Serial.cpp
	)
set(ShenHongYT_17th "ShenHongYT_17th")
add_executable(${ShenHongYT_17th} ${VISION} ${CONTROL} ${SERIAL} ${MAIN})
target_link_libraries(${ShenHongYT_17th} ${PADDLELITE_LIBRARY})
target_link_libraries(${ShenHongYT_17th} ${OpenCV_LIBS} -lpthread -lm) 
#target_link_libraries(${ShenHongYT_17th} PRIVATE pthread)
#target_link_libraries(${ShenHongYT_17th} ${serial})