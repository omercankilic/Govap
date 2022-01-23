#ifndef VIDEOCONTROL_H
#define VIDEOCONTROL_H

//Developer designed libs
#include "helper_functions.h"
//C++ libs
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <vector>
#include <deque>
//Qt libs
#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>

enum VC_ERRORS{
    FMTX_INITIALIZATION_ERROR,
    EOFFILE,
    UNDEFINED_ERROR
    
};

struct sliderInit{
        int thick_interval;
        int min_val;
        int max_val;
};

class VideoControl:public QObject
{
        Q_OBJECT
    public:
        VideoControl();
        
        //State variables
        atomic<bool> is_video_selected;
        atomic<bool> is_video_exist  ;
        atomic<bool> is_audio_exist  ;
        atomic<bool> is_video_playing;
        atomic<bool> is_video_paused ;
        atomic<bool> is_video_stopped;
        atomic<bool> is_end_of_file;
        atomic<int>  playing_delay;
        atomic<int>  current_frame_index_inQ;
        atomic<bool> seek_flag;
        atomic<int> seek_time_from_slider;
        
        //mutexes for state controls
        mutex mx_video_pause;
        mutex mx_video_stopped;
        //condition variables for waiting next states
        condition_variable cv_video_pause;
        
        //Video info parameters
        AVRational   input_video_framerate;
        atomic<int>  video_stream_index;
        atomic<int>  audio_stream_index;
        atomic<int>  playing_speed_coeff;
        uint64_t     video_duration;
        uint64_t     last_frame_pts;
        uint64_t     current_frame_pts;
        int          input_video_gop_size;
        
        //Input video parameters
        QString input_file_url;
        //AVFormat and AVCodecContext degiskenleri
        AVFormatContext *inputFileFormatCtx  = nullptr;
        AVCodecContext  *video_decoder_ctx = nullptr;
        AVCodecContext  *audio_decoder_ctx = nullptr;
        AVCodecContext  *video_encoder_ctx = nullptr;
        AVCodec         *audio_decoder_codec   = nullptr;   /**@todo memory control will be checked*/
        AVCodec         *video_encoder_codec   = nullptr;   /**@todo memory control will be checked*/
        AVCodec         *video_decoder_codec   = nullptr;   /**@todo memory control will be checked*/
        AVStream        *video_stream          = nullptr;
        AVStream        *audio_stream          = nullptr;
            
        //AVFormat and AVCodecContext fonksiyonlari
        void initialize_parameters();        
        int initialize_input_format_ctx();
        int initialize_video_decoder_ctx();
        int initialize_audio_decoder_ctx();
        //int initialize_output_encoder_ctx();
        void decode_and_show_packet(int show_opt, AVPacket* current_packet);
        AVPacket* read_one_packet();
        void flush_video_decoder();
        void flush_audio_decoder();
               
        void start_playing_video();
        void show_frame(AVFrame *frame);
        void clean_video_decoder();
        void clean_audio_decoder();
        void clean_format_ctx();
        void clean_all();
        void reset_all();
        void set_slider_pars();
        
        std::thread *th_video_playing;
        mutex mx_frames;
        deque<cv::Mat> cv_mat_framesQ;
    private:
        int64_t current_frame_number;
        int64_t get_seek_timestamp();
        void initialize_sig_conn();
    public slots:
        
        void on_fileButton();
        int on_startVideo();
        void stop_video();
        void on_nextFrame();
        void on_previousFrame();
        void on_videoPaused();
        void on_seek2_frame(int seek_slider_val);
        
    signals:
        void sig_send_new_frame(quint64 frame_index);
        void send_file_name(QString);
        void sig_slider_value_set(qint64 slider_val);
        void sig_initialize_slider(void *slider_init_par);
        void sig_clean_all();
        void sig_pause_button_state(bool but_state);// if state == 1 pause if state == 0 continue
        
};

#endif // VIDEOCONTROL_H
