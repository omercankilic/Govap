#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Govap");
    initialize_parameters();
    initialize_signal_connections();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::initialize_parameters(){
    this->scne = new QGraphicsScene();
    this->ui->mainVideo->setScene(scne);
    videoCont = new VideoControl();
    painterCont = new PainterController();
}

void MainWindow::initialize_signal_connections(){  
    connect(this,&MainWindow::on_fileButtonSig,this->videoCont,&VideoControl::on_fileButton);
    connect(this,&MainWindow::on_nextFrameSig,this->videoCont,&VideoControl::on_nextFrame);
    connect(this,&MainWindow::on_startVideoSig,this->videoCont,&VideoControl::on_startVideo);
    connect(this,&MainWindow::on_videoPausedSig,this->videoCont,&VideoControl::on_videoPaused);
    connect(this,&MainWindow::on_previousFrameSig,this->videoCont,&VideoControl::on_previousFrame);
    connect(this,&MainWindow::on_stopVideoSig,this->videoCont,&VideoControl::stop_video);
    connect(this->videoCont,&VideoControl::send_file_name,this,&MainWindow::file_name_set);
    connect(this->videoCont,&VideoControl::sig_initialize_slider,this,&MainWindow::initialize_slider);
    connect(this->videoCont,&VideoControl::sig_slider_value_set,this,&MainWindow::set_slider_value);
    connect(this->videoCont,&VideoControl::sig_send_new_frame,this,&MainWindow::view_frame);
    connect(this->videoCont,&VideoControl::sig_pause_button_state,this,&MainWindow::set_pause_but_state);
    connect(this,&MainWindow::on_createPainter,this->painterCont,&PainterController::create_new_painter);
}

void MainWindow::file_name_set(QString file_name){
    //This signal can be deleted or be used to change file name in main window
}


void MainWindow::set_slider_value(qint64 slider_val){
    ui->videoSeekSlider->setValue(slider_val);
}

void MainWindow::initialize_slider(void *slider_init_par){
    sliderInit *temp_par = (sliderInit*)slider_init_par;
    ui->videoSeekSlider->setMinimum(temp_par->min_val);
    ui->videoSeekSlider->setMaximum(temp_par->max_val);
    ui->videoSeekSlider->setTickInterval(temp_par->thick_interval);
    cout<<ui->videoSeekSlider->maximum()<<endl;
    cout<<ui->videoSeekSlider->minimum()<<endl;
    
}

void MainWindow::set_pause_but_state(bool but_state){
    if(true == but_state){
        ui->pauseVideo->setIcon(QIcon(":/resources/icons/pause.png"));
    }else{
        ui->pauseVideo->setIcon(QIcon(":/resources/icons/play.png"));
    }
}
//----------------------------------------------------------//
void MainWindow::view_frame(quint64 frame_index)
{
    cv::Mat mat_img;
    {
        unique_lock<mutex> lk(videoCont->mx_frames);
        if(false == videoCont->cv_mat_framesQ.empty()){
            if(10 == frame_index){
                cv::cvtColor(videoCont->cv_mat_framesQ.back(), mat_img, cv::COLOR_BGR2RGB);
            }else{
                cv::cvtColor(videoCont->cv_mat_framesQ.at(frame_index), mat_img, cv::COLOR_BGR2RGB);
            }
            
        }else{
            return;
        }
    }
    QImage qimg((uchar *)mat_img.data, mat_img.cols, mat_img.rows, mat_img.step, QImage::Format_RGB888);
    current_image = mat_img.clone();
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(scale_QImg(ui->mainVideo->width(),ui->mainVideo->height(),qimg)));
    if(ui->mainVideo->scene()->items().size()>0){
        ui->mainVideo->scene()->clear();
    }
    ui->mainVideo->scene()->addItem(item);
}


void MainWindow::on_nextFrame_clicked(){
    emit on_nextFrameSig();
}

void MainWindow::on_pauseVideo_clicked(){
    emit on_videoPausedSig();
}

void MainWindow::on_actionOpen_triggered(){
    emit on_fileButtonSig(); 
}

void MainWindow::on_videoSeekSlider_sliderMoved(int position){
    
}

void MainWindow::on_previousFrame_clicked(){
    emit on_previousFrameSig();
}

void MainWindow::on_stopVideo_clicked(){
    emit on_stopVideoSig();
}

void MainWindow::on_actionEdit_Frame_triggered(){
    cout<<"test"<<endl;
    emit on_createPainter((void*)(&current_image));
}
