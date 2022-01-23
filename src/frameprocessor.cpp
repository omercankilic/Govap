#include "frameprocessor.h"

void FrameProcessor::delete_slot()
{
    cout<<"delete slot is called"<<endl;
}

void FrameProcessor::create_new_scribble(void* current_frame){
    if(MAX_SCRIBBLE_WINDOW_ARRAY_SIZE > active_scribble_window_nb){
        if(false == (*(cv::Mat*)(current_frame)).empty()){
            
            for (int var = 0; var < MAX_SCRIBBLE_WINDOW_ARRAY_SIZE; var++) {
                
                if(false == window_vects[var]->is_active){
                    window_vects[var]->set_pars(current_frame);
                    window_vects[var]->show();
                    active_scribble_window_nb++;
                    break;
                }
            } 
            
        }
    }    
}

FrameProcessor::FrameProcessor(){
    initialize_parameters();
}

void FrameProcessor::initialize_parameters(){
    
    active_scribble_window_nb = 0;
    for (int var = 0; var < MAX_SCRIBBLE_WINDOW_ARRAY_SIZE; var++) {
        window_vects[var] = new ScribbleWindow(var);
    }
}
