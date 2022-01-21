#ifndef FrameProcessor_H
#define FrameProcessor_H

#define MAX_SCRIBBLE_WINDOW_ARRAY_SIZE 5

#include <QObject>
#include <QWidget>
#include <iostream>
#include "helper_functions.h"
#include "scribblewindow.h"

using namespace std;
class FrameProcessor:public QObject{
        Q_OBJECT

    public slots:
        void create_new_scribble(void* current_frame);
        
    public:
        FrameProcessor();
    private:
        uint8_t active_scribble_window_nb;
        void initialize_parameters();
        ScribbleWindow* scribble_window_array[MAX_SCRIBBLE_WINDOW_ARRAY_SIZE];
};

#endif // FrameProcessor_H
