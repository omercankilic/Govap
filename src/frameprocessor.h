#ifndef FrameProcessor_H
#define FrameProcessor_H

#define MAX_SCRIBBLE_WINDOW_ARRAY_SIZE 5

#include <QObject>
#include <QWidget>
#include <iostream>
#include <vector>
#include "helper_functions.h"
#include "scribblewindow.h"


using namespace std;
class FrameProcessor:public QObject{
        Q_OBJECT

    public slots:
        void delete_slot();
        void create_new_scribble(void* current_frame);
    public:
        FrameProcessor();
    private:
        void initialize_parameters(); 
        ScribbleWindow* window_vects[MAX_SCRIBBLE_WINDOW_ARRAY_SIZE];
};

#endif // FrameProcessor_H
