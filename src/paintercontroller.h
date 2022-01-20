#ifndef PAINTERCONTROLLER_H
#define PAINTERCONTROLLER_H

#include <QObject>
#include <QWidget>
#include "scribblearea.h"

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

#include <iostream>
using namespace std;
class PainterController:public QObject{
        Q_OBJECT

    public slots:
        void create_new_painter(void* current_frame);
        
    public:
        PainterController();
    private:
        uint8_t active_painter_nb;
        void initialize_parameters();
        ScribbleArea* scribble_array[5];
};

#endif // PAINTERCONTROLLER_H
