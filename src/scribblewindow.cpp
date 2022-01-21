#include "scribblewindow.h"
#include "ui_scribblewindow.h"
#include <QtWidgets>
ScribbleWindow::ScribbleWindow(void* current_frame_ptr,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScribbleWindow){
    
    //here we clone the current frame in the main window class,
    //since it may change while we are processing the cv::Mat frame
    ui->setupUi(this);
    
    this->current_frame = (*((cv::Mat*)(current_frame_ptr))).clone();
    QImage qimg((uchar *)this->current_frame.data, this->current_frame.cols, this->current_frame.rows, this->current_frame.step, QImage::Format_RGB888);
    
    this->scribble_area = new ScribbleArea(qimg);
    setCentralWidget(this->scribble_area);
    this->connect_actions();
    
    this->show();
}

ScribbleWindow::~ScribbleWindow(){
    delete ui;
    cout<<"scriible close "<<endl;
}

void ScribbleWindow::on_actionClean_Screen_triggered(){
    this->scribble_area->clearImage();
}

void ScribbleWindow::on_actionPen_Width_triggered(){
    bool check;
    int new_pen_width = QInputDialog::getInt(this,tr("Scribble"),tr("Choose pen width:"),this->scribble_area->penWidth(),1,52,1,&check);
    if(true == check){
        this->scribble_area->setPenWidth(new_pen_width);
    }
}

void ScribbleWindow::on_actionPen_Color_triggered(){
    QColor new_pen_color = QColorDialog::getColor(this->scribble_area->penColor());
    if(new_pen_color.isValid()){
        this->scribble_area->setPenColor(new_pen_color);
    }
    
}

void ScribbleWindow::connect_actions(){
    //Connecting save actions.
    foreach (QAction *this_action, this->ui->menuSave_As->actions()){
        this_action->setData(this_action->toolTip().toLower().toUtf8());
        connect(this_action,SIGNAL(triggered()),this,SLOT(save()));
    }
}

void ScribbleWindow::save(){
    QAction *action = qobject_cast<QAction *>(sender());

    QByteArray fileFormat = action->data().toByteArray();
    save_file(fileFormat);
}

bool ScribbleWindow::save_file(const QByteArray &fileFormat){

    cout<<"save file"<<endl;
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;    
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));
    if (fileName.isEmpty()) {
        return false;
    } else {
        return this->scribble_area->saveImage(fileName, fileFormat.constData());
    }

}




void ScribbleWindow::on_actionback_triggered(){
    this->scribble_area->go_back();
}

void ScribbleWindow::on_actionforward_triggered(){
    this->scribble_area->go_forward();
}
