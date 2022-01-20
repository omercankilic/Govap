#include "graphicsdraw.h"

void MyGraphicsDraw::clearImage()
{
    
}

void MyGraphicsDraw::printImage(const QString file_name)
{
    
}

void MyGraphicsDraw::setDrawingState(const bool drawingState)
{
    drawing_active = drawingState;
}

void MyGraphicsDraw::setOnSceneState(const bool onSceneState)
{
    on_scene = onSceneState;
}

void MyGraphicsDraw::mousePressEvent(QMouseEvent *event)
{
    if(true == drawing_flag){
        
        QPoint temp_mouse_point = event->pos();
        //This if statement is to check whether press event is in scene borders
        if(/*temp_mouse_point.x() <= this->draw_graphics_view->rect().x2 && 
           temp_mouse_point.x() >=this->draw_graphics_view->rect().x1  &&
           temp_mouse_point.y() >=this->draw_graphics_view->rect().y1  &&
           temp_mouse_point.y() <=this->draw_graphics_view->rect().y2 ||*/ true)
        {
            if(event->button() == Qt::LeftButton){
                last_point = event->pos();
                drawing_active = true;
            }
            
        }else{
            //Logging here
            cout<<"press is not in scene border"<<endl;
        }
    }else{
        //Log here
        cout<<"main drawing flag is not active"<<endl;
    }
}

void MyGraphicsDraw::mouseMoveEvent(QMouseEvent *event)
{
    
}

void MyGraphicsDraw::mouseReleaseEvent(QMouseEvent *event)
{
    
}

void MyGraphicsDraw::paintEvent(QPaintEvent *event)
{
    
}

MyGraphicsDraw::MyGraphicsDraw(QWidget *parent,QGraphicsView *main_graphics_view):QWidget(parent)
{
    draw_graphics_view = main_graphics_view;
    modified = false;
}
