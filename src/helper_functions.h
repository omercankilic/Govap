#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImage>

#include <iostream>

#include "ffmpeg_headers.h"
using namespace cv;
using namespace std;

void ffmpeg_hata_print(const int hata_flag);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
QImage scale_QImg(int width,int height,QImage &in_img);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void avframeToMat(const AVFrame *frame, Mat& image);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
AVFrame* avframeToAvframe(const AVFrame *in_frame,AVPixelFormat target_pix_fmt,AVCodecContext *enc_ctx);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int show_error(int error_key);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
long double get_current_time_millisecond();
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#endif // HELPER_FUNCTIONS_H

