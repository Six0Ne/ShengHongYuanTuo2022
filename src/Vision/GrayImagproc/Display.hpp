#pragma once
#ifndef DISPLAY_H_H
#define DISPLAY_H_H

extern Mat  Disp_gray, Disp_bin, Disp_edge;
extern int disp_x,disp_y;
void vision_display(Mat& image_disp,Mat& imgae_smask);
#endif // DISPLAY_H_H