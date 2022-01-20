#ifndef GRAPHICSDRAW_H
#define GRAPHICSDRAW_H
#include <QGraphicsView>
#include <QWidget>
#include <iostream>
#include <QMouseEvent>
using namespace std;


class MyGraphicsDraw:public QWidget
{
        Q_OBJECT
    public:
    
        MyGraphicsDraw(QWidget *parent = nullptr,QGraphicsView *main_graphics_view = nullptr);
        void setPenColor(const QColor new_pen_color);
        void setPenWidth(const int new_pen_width);
        
    public slots:
        void clearImage();
        void printImage(const QString file_name);
        void setDrawingState(const bool drawingState);
        void setOnSceneState(const bool onSceneState);
        
    protected:
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void paintEvent(QPaintEvent *event) override;
        
        
    private:
        QGraphicsView *draw_graphics_view;
        QPoint last_point;
        QColor active_pen_color = Qt::black;
        int    active_pen_width = 1;
        
        bool modified        = false;
        bool drawing_active  = false;
        bool drawing_flag    = true;
        bool on_scene        = false;
        
        void drawLineOnImage(const QPoint &end_point);
        
};

#endif // GRAPHICSDRAW_H
