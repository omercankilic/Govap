#ifndef FFMPEG_HEADERS_H
#define FFMPEG_HEADERS_H

extern "C"{
    #include    <libavcodec/avcodec.h>
    #include    <libavformat/avformat.h>
    #include    <libavutil/avconfig.h>
    #include    <libavdevice/avdevice.h>
    #include    <libavfilter/avfilter.h>
    #include    <libswscale/swscale.h>
    #include    <libswresample/swresample.h>
    #include    <libavutil/imgutils.h>  
}


//void ffmpeg_hata_print(const int hata_flag){
//    char error_str[200];
//    av_strerror(hata_flag,error_str,200);
//    cout<<"HATA : "<<error_str<<endl;
//}
#endif // FFMPEG_HEADERS_H
