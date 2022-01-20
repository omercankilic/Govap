#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//Qt libraries
#include <QEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QFileDialog>
#include "paintercontroller.h"
//C++ libraries

//developer created header files
#include "graphicsdraw.h"
#include "videocontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
        Q_OBJECT
        
    signals:
        void onSceneSignal(bool is_on_scene);
        void drawingActivationSignal(bool is_drawing_active);
        
        void on_fileButtonSig();
        void on_startVideoSig();
        void on_nextFrameSig();
        void on_previousFrameSig();
        void on_videoPausedSig();
        void on_stopVideoSig();
        void on_createPainter(void* current_frame);
        
    public:
        MainWindow(QWidget *parent = nullptr);
        VideoControl *videoCont;
        PainterController *painterCont;
        
        ~MainWindow();
        
        void initialize_parameters();        
        void initialize_signal_connections();
        
        //Video window scene parameters
        QGraphicsScene *scne;
        cv::Mat current_image;
    public slots:
        void file_name_set(QString file_name);
        void set_slider_value(qint64 slider_val);
        void initialize_slider(void *slider_init_par);
        void set_pause_but_state(bool state);
    private slots:
        void view_frame(quint64 frame_index);
        void on_nextFrame_clicked();
        void on_pauseVideo_clicked();
        
        //Menubar actions
        void on_actionOpen_triggered();
        
        void on_videoSeekSlider_sliderMoved(int position);
        
        void on_previousFrame_clicked();
    
        void on_stopVideo_clicked();
        
        void on_actionEdit_Frame_triggered();
        
    private:
        MyGraphicsDraw *mwGraphicsDraw;
        Ui::MainWindow *ui;
        
        
};

#endif // MAINWINDOW_H
