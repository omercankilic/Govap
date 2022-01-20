#include "paintercontroller.h"

void PainterController::create_new_painter(void* current_frame){
    
    if(5 > this->active_painter_nb){
        if(false == (*(cv::Mat*)(current_frame)).empty()){
        cout<<"painter controller func"<<endl;
            this->active_painter_nb ++;
            QWidget *temp_widget = new QWidget();
            this->scribble_array[active_painter_nb] = new ScribbleArea(*(cv::Mat*)(current_frame),temp_widget);
        }
    }    
}

PainterController::PainterController(){
    this->initialize_parameters();
    
}

void PainterController::initialize_parameters(){
    active_painter_nb = 0 ;
}
