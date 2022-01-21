#include "frameprocessor.h"

void FrameProcessor::create_new_scribble(void* current_frame){
    
    if(MAX_SCRIBBLE_WINDOW_ARRAY_SIZE > this->active_scribble_window_nb){
        if(false == (*(cv::Mat*)(current_frame)).empty()){
        cout<<"painter controller func"<<endl;
            this->scribble_window_array[active_scribble_window_nb] = new ScribbleWindow(current_frame);
            this->active_scribble_window_nb++;
        
        }
    }    
}

FrameProcessor::FrameProcessor(){
    this->initialize_parameters();
    
}

void FrameProcessor::initialize_parameters(){
    active_scribble_window_nb = 0 ;
}
