#include "videocontrol.h"

VideoControl::VideoControl(){
    
}

void VideoControl::initialize_parameters(){
    is_video_paused   = false;
    is_video_playing  = false;
    is_video_stopped  = false;
    is_end_of_file    = false;
    is_video_exist    = false;//decided in initialize_input_format_ctx func
    is_audio_exist    = false;//decided in initialize_input_format_ctx func
    current_frame_index_inQ = -1;
    current_frame_number = 0;
    input_video_gop_size = -1;//decided in initialize_decoder_ctx func
    playing_speed_coeff  =  1;//will be connected to UI
    playing_delay        = -1;//decided in initialize_decoder_ctx func
    video_stream_index   = -1;//decided  in initialize_input_format_ctx func
    audio_stream_index   = -1;//decided in initialize_input_format_ctx func
    current_frame_pts    = 0; // last played frame pts 
    video_duration       = 0; // video duration calculated by hand = 
    last_frame_pts       = 0; // pts of the last frame in the queue
    input_video_framerate.den = 0;
    input_video_framerate.num = 0;
    seek_time_from_slider = 0;
    seek_flag = false;
}

int VideoControl::initialize_input_format_ctx()
{
    int ret;
    if(nullptr != inputFileFormatCtx){
        cout<<"A video is already playing"<<endl;
    }
    if(0 == input_file_url.size()){
        cout<<"input_file_url is empty"<<endl;
        return -1;
    }
    inputFileFormatCtx = avformat_alloc_context();
    
    ret = avformat_open_input(&inputFileFormatCtx,(const char*)(this->input_file_url.toStdString().c_str()),NULL,NULL);
    if(ret <0){
        //ffmpeg_hata_print(ret);
        return -1;
    }else{
        // Writing the information of input file format into the console.
        av_dump_format(inputFileFormatCtx,0,input_file_url.toStdString().c_str(),0);
        
    }
    ret = avformat_find_stream_info(inputFileFormatCtx,NULL); 
    if(ret<0){ 
        cout<<"NO STREAM INFO FOUND"<<endl;
        return -1;
    }
    for(int i = 0;i < (int)inputFileFormatCtx->nb_streams;i++){
        if(AVMEDIA_TYPE_VIDEO == inputFileFormatCtx->streams[i]->index){
            video_stream_index = i;
            is_video_exist = true;
            video_stream = inputFileFormatCtx->streams[video_stream_index];
             video_duration = inputFileFormatCtx->duration;
            cout<<"duration : "<<video_duration<<endl;
        }else if(AVMEDIA_TYPE_AUDIO == inputFileFormatCtx->streams[i]->index){
            audio_stream_index = i;
            audio_stream = inputFileFormatCtx->streams[audio_stream_index];
            is_audio_exist = true;
        }
    }
    
    if(-1 == video_stream_index){
        cout<<"NO VIDEO STREAM"<<endl;
    }
    if(-1 == audio_stream_index){
        cout<<"NO AUDIO STREAM"<<endl;
    }
    return 0;
}
int VideoControl::initialize_video_decoder_ctx()
{
    video_decoder_codec = avcodec_find_decoder( inputFileFormatCtx->streams[video_stream_index]->codecpar->codec_id);
    if(nullptr == video_decoder_codec){
        cout<<"DECODER CODEC CAN NOT BE FOUND"<<endl;
        return -1;
    }
    video_decoder_ctx = avcodec_alloc_context3(video_decoder_codec);
    if(nullptr == video_decoder_ctx ){
        cout<<"video_decoder_ctx can not be allocated"<<endl;
        return -1;
    }
    if(avcodec_parameters_to_context(video_decoder_ctx,inputFileFormatCtx->streams[video_stream_index]->codecpar)<0){
        cout<< "codec parameters could not be passed"<<endl;        
        return -1;
    }
    
    if(video_decoder_ctx->codec_type == AVMEDIA_TYPE_VIDEO){
        video_decoder_ctx->framerate = av_guess_frame_rate(inputFileFormatCtx,inputFileFormatCtx->streams[video_stream_index],NULL);
        if( avcodec_open2(video_decoder_ctx,video_decoder_codec,NULL)<0){
            cout<<"s_codec context acilamadi"<<endl;
            return -1;
        }
    } 
    
    input_video_gop_size = video_decoder_ctx->gop_size;
    if(0 >= video_decoder_ctx->framerate.den || 0 >= video_decoder_ctx->framerate.num){
        /***
         * We use denominator value in division processes. So, if it is 0,
         * frame rate of the video to play will be automatically defined
         * as 25 FPS. We also set this value to 25 FPS in any inconvenient
         * frame rate value for video.
         * */
        input_video_framerate.num = 25; 
        input_video_framerate.den = 1;
    }else{    
        input_video_framerate.den = video_decoder_ctx->framerate.den;
        input_video_framerate.num = video_decoder_ctx->framerate.num;
    }
    playing_delay        = (int)(1000*(int)(1000/input_video_framerate.num)/playing_speed_coeff);
    cout<<"time base : "<<video_decoder_ctx->time_base.num <<"/"<<video_decoder_ctx->time_base.den<<endl;
    cout<<"gop size  : "<<video_decoder_ctx->gop_size<<endl;
    return 0;
}

int VideoControl::initialize_audio_decoder_ctx()
{
    if(audio_stream_index <0){
        cout<<"CAN NOT INITIALIZE AUDIO DECODER CTX SINCE NO AUDIO STREAM"<<endl;
        return -1;
    }
    audio_decoder_codec = avcodec_find_decoder(inputFileFormatCtx->streams[audio_stream_index]->codecpar->codec_id);
    if(audio_decoder_codec == nullptr){
        cout<<"NO AUDIO CODEC FOUND FOR THIS VIDEO"<<endl;
        return -1;
    }
    audio_decoder_ctx = avcodec_alloc_context3(audio_decoder_codec);
    if(audio_decoder_ctx == nullptr){
        cout<<"AUDIO DECODER CTX CAN NOT BE ALLOCATED"<<endl;
        return -1;
    }
    if(avcodec_parameters_to_context(audio_decoder_ctx,inputFileFormatCtx->streams[audio_stream_index]->codecpar)<0){
        cout<<"CODEC PARAMETERS CAN NOT BE PASSED TO AUDIO CTX"<<endl;
        return -1;
    }
    
    return 0;
}

void VideoControl::start_playing_video()
{
    while(true == is_video_playing && false == is_video_stopped){
        
        if(seek_flag){
            seek_flag =false;
            
            {
                unique_lock<mutex> lk(this->mx_frames);
                if(false == cv_mat_framesQ.empty()){
                    cv_mat_framesQ.clear();
                }
            }
            current_frame_number = seek_time_from_slider/1000;
            current_frame_number = current_frame_number/this->video_decoder_ctx->framerate.den;
            
            avcodec_flush_buffers(this->video_decoder_ctx);
            avcodec_send_packet(this->video_decoder_ctx,NULL);
            //avcodec_send_packet(this->audio_decoder_ctx,NULL);
            
            int ret = av_seek_frame(inputFileFormatCtx,video_stream_index,seek_time_from_slider,AVSEEK_FLAG_BACKWARD);
            cout<<"Seek is succesful : "<<ret<<endl;            
            
        }
        if(true == is_video_paused){
            cout<<"Video paused"<<endl;
            unique_lock<mutex> lk(mx_video_pause);
            cv_video_pause.wait(lk,[&]{
                return false == is_video_paused || true == is_video_stopped ||true == seek_flag;
            });
            
            if(true == is_video_stopped){
                /**
                 * If is_video_stopped is true there are 2 options
                 * 1: video is really stopped
                 * 2: new video will be played while one video is still playing
                 */
                clean_all();
                break;
            }
        }
        
        AVPacket *temp_pkt = read_one_packet();
        if(nullptr == temp_pkt){
            //here we understand that the end of file error came frome read_one_packet function
            is_video_playing = false;
            is_end_of_file   = true;
            decode_and_show_packet(-1,temp_pkt);
            break;
        }else{
            decode_and_show_packet(0,temp_pkt);
        }
    }
    
    if(is_video_stopped){
        clean_all();
        return;
    }
    if(true == is_end_of_file){
        /**
          * @todo bring the application to initial state.
          */
        reset_all();
    }
}

void VideoControl::show_frame(AVFrame *current_frame)
{
    usleep(playing_delay/playing_speed_coeff);
    cv::Mat img;
    avframeToMat(current_frame,img);
    {
        unique_lock<mutex> lk(this->mx_frames);
        cv_mat_framesQ.push_back(img.clone());
        if(cv_mat_framesQ.size()>11){
            cv_mat_framesQ.pop_front();
        }
    }
    if(cv_mat_framesQ.size()<10){
        current_frame_index_inQ +=1;
    }
    
    current_frame_number++;
    qint64 temp_slider_val = 1000*(current_frame_number)*1000*(double)((double)input_video_framerate.den/(double)input_video_framerate.num);
    emit sig_slider_value_set(temp_slider_val);
    emit sig_send_new_frame(10);
    av_frame_free(&current_frame);
}

void VideoControl::clean_format_ctx()
{
    if(nullptr != inputFileFormatCtx){
        avformat_close_input(&inputFileFormatCtx);
    }
}

void VideoControl::clean_all(){
    clean_video_decoder();
    clean_audio_decoder();
    clean_format_ctx();
        
    {
        unique_lock<mutex> lk(this->mx_frames);
        if(false == cv_mat_framesQ.empty()){
            cv_mat_framesQ.clear();
        }
    }

}

void VideoControl::reset_all(){
    clean_all();
    initialize_parameters();
    emit sig_slider_value_set(0);
}

void VideoControl::set_slider_pars(){
    sliderInit sld_pars;
    
    sld_pars.thick_interval = video_stream->time_base.den/video_decoder_ctx->framerate.num*1000;    
    sld_pars.max_val= video_duration;
    sld_pars.min_val= 0;
    emit sig_initialize_slider((void*)&sld_pars);
}

void VideoControl::clean_video_decoder(){
    if(nullptr != video_decoder_ctx){
        avcodec_free_context(&video_decoder_ctx);
        video_decoder_ctx = nullptr;
        video_stream = nullptr;
    }
}

void VideoControl::clean_audio_decoder(){
    if(nullptr!= audio_decoder_ctx){
        avcodec_free_context(&audio_decoder_ctx);
        audio_decoder_ctx = nullptr;
        audio_stream = nullptr;
    }
    
}
AVPacket* VideoControl::read_one_packet()
{
    if(nullptr == inputFileFormatCtx){
        cout<<"inputFileFormatCtx IS NULLPTR"<<endl;
        return nullptr;
    }
    int ret;
    AVPacket *current_packet;
    current_packet = av_packet_alloc();
    
    ret = av_read_frame(inputFileFormatCtx,current_packet);
    
    cout<<"pts : "<<current_packet->pts<<endl;
    if(AVERROR_EOF == ret){
        cout<<"END OF FILE ERROR"<<endl;
        return nullptr;
    }
    if(ret < 0){
        ffmpeg_hata_print(ret);
        return nullptr;
    }else{
        return current_packet;
    }
}

void VideoControl::flush_video_decoder(){
    //avcodec_flush_buffers(video_decoder_ctx);
}
/***
 *  @show_opt: defines the option of showing
 *  if show_opt == 0, show the frames got from packet
 *  if show_opt == 1, decode packet and add frame to Queue
 *  if show_opt == 2, decode and skip the frame
 *  
 * @current_packet: packet read from the file
 *  if current_packet == nullptr 
 *  this means the end of file and we initialize the structure again by
 *  cleaning the VideoControl class. In this condition show_opt is also
 *  given as -1.
*/
void VideoControl::decode_and_show_packet(int show_opt, AVPacket *current_packet)
{
    int ret = -1;
    if(nullptr == current_packet){
        clean_all();
        return;
    }
    if(video_stream_index == current_packet->stream_index){
        ret = avcodec_send_packet(video_decoder_ctx,current_packet);
        if(0 > ret){
            av_packet_free(&current_packet);
        }
        while(ret>=0){
            AVFrame  *current_frame;
            current_frame  = av_frame_alloc();            
            ret = avcodec_receive_frame(video_decoder_ctx,current_frame);
            
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){   
                av_frame_free(&current_frame);
                current_frame = nullptr;                
                break;
            }else if (ret < 0) {
                ffmpeg_hata_print(ret);
                av_frame_free(&current_frame);                
                cout<<"Error in decoding packet"<<endl;
                break;
            }
            if(0 == show_opt){
                show_frame(current_frame);
             }else if(1 == show_opt){
                cv::Mat temp_mat;
                avframeToMat(current_frame,temp_mat);
                {
                    unique_lock<mutex> lk(this->mx_frames);
                    cv_mat_framesQ.push_back(temp_mat.clone());
                    if(cv_mat_framesQ.size()>11){
                        cv_mat_framesQ.pop_front();
                    }
                }
            }else if(2 == show_opt){
                av_frame_free(&current_frame);
            }
        }
    }else{
        //cout<<"NOT A VIDEO PACKET"<<endl;
        av_packet_free(&current_packet);
        current_packet = nullptr;
    }
    
}

void VideoControl::on_fileButton()
{
    QWidget *temp_widget = new QWidget();
    QString file_name = QFileDialog::getOpenFileName(temp_widget,"Select file to play.","/home/mrcan/Desktop");
    if(file_name.isEmpty()){
        return;
    }else{
        cout<<"file name : "<<file_name.toStdString()<<endl;
        input_file_url = file_name;        
        if(0 > on_startVideo()){
            QMessageBox message_box;
            message_box.critical(0,"Error"," File can't be played.");
            message_box.setFixedSize(500,200);
        }
        
    }
}

int VideoControl::on_startVideo(){
    if(true == is_video_playing){
        if(true == is_video_paused){
            is_video_stopped = true;
            cv_video_pause.notify_all();
        }else{
            is_video_stopped = true;
        }
        this->th_video_playing->join();
        delete th_video_playing;
        clean_all();
    }
    
    initialize_parameters();
    
    if(0 > initialize_input_format_ctx()){
        return -1;
    }
    

    if(0 > initialize_video_decoder_ctx()){
        return -1;   
    }

    
    set_slider_pars();
    /**
     * @todo we don't process audio data, so audio decoder will
     * not be used no audio decoding will be done in this version.
     * TO BE DEVELOPED IN THE FUTURE.
     * 
     * if(0 > initialize_audio_decoder_ctx()){
     *     return -1;
     * }
     * 
     * 
     */
    
    is_video_playing = true;
    is_video_stopped = false;
    th_video_playing = new std::thread([this](){start_playing_video();});
    emit sig_pause_button_state(true);
    return 0;
}

void VideoControl::stop_video(){
    this->on_startVideo();
}

void VideoControl::on_nextFrame()
{
    if(true == is_video_paused){
        
        if(current_frame_index_inQ < (int)cv_mat_framesQ.size()-1 && current_frame_index_inQ >=0){
            current_frame_index_inQ +=1;
            current_frame_number +=1;
            qint64 temp_slider_val = 1000*(current_frame_number)*1000*((double)input_video_framerate.den/(double)input_video_framerate.num);
            emit sig_slider_value_set(temp_slider_val);
            emit sig_send_new_frame(current_frame_index_inQ);
           
        }else if(current_frame_index_inQ == (int)cv_mat_framesQ.size()-1){
            AVPacket *tmp;
            do{
                tmp = read_one_packet();
                if(nullptr == tmp){
                    decode_and_show_packet(-1,tmp);
                    return;
                }
            }while(video_stream_index != tmp->stream_index);
            decode_and_show_packet(0,tmp);    
        }
    }
}

int64_t VideoControl::get_seek_timestamp(){
    return (int64_t(current_frame_number)*1000*input_video_framerate.den/input_video_framerate.num);
}

void VideoControl::initialize_sig_conn(){
    /***
     * @todo will be implemented.
      */
    
}

void VideoControl::on_previousFrame()
{
    if(true == is_video_paused){
        if(cv_mat_framesQ.size()>=1){
            if(current_frame_index_inQ > 0){
                current_frame_index_inQ -=1;
                current_frame_number -=1;
                qint64 temp_slider_val = 1000*(current_frame_number)*1000*((double)input_video_framerate.den/(double)input_video_framerate.num);
                emit sig_slider_value_set(temp_slider_val);                
                emit sig_send_new_frame(current_frame_index_inQ);
            }else if(0 == current_frame_index_inQ){
                current_frame_number = current_frame_number-1;
                int64_t timestamp_ts = get_seek_timestamp();
                cout<<"seek time stamp: " << timestamp_ts<<endl;
                int ret = av_seek_frame(inputFileFormatCtx,video_stream_index,timestamp_ts,AVSEEK_FLAG_BACKWARD);
                cout<<"Seek is succesful : "<<ret<<endl;
                do{
                    AVPacket *tmp;
                    
                    tmp = read_one_packet();
                    //cout<<"prev packet pts : "<<tmp->pts<<endl;
                    
                    if(tmp->pts<timestamp_ts){
                        decode_and_show_packet(1,tmp);
                    }else{
                        decode_and_show_packet(1,tmp);
                        current_frame_index_inQ = cv_mat_framesQ.size()-2;
                        qint64 temp_slider_val = 1000*(current_frame_number-1)*1000*((double)input_video_framerate.den/(double)input_video_framerate.num);
                        emit sig_slider_value_set(temp_slider_val);                                        
                        emit sig_send_new_frame(current_frame_index_inQ);
                        break;
                    }
                }while(true); 
            }
        }
    }
}

void VideoControl::on_videoPaused(){   
    if(true == is_video_playing){
        
        if(false == is_video_paused){
            is_video_paused = true;
            emit sig_pause_button_state(false);
        }else{
            /**
             *Bu kisima eger suan videoyu oynatirken ileri geri yapilmis ve de queue icerisinde
             * ortalarda bir yerdeysek, oncelikle queue sonuna gelene kadar oynatma islemi yapacak bir
             * fonksiyon koyulacak
             */
            is_video_paused = false;
            emit sig_pause_button_state(true);
            cv_video_pause.notify_all();
        }
    }
}

void VideoControl::on_seek2_frame(int seek_slider_val){
    seek_flag = true;
    seek_time_from_slider = seek_slider_val;
    
}
