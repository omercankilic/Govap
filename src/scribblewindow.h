#ifndef SCRIBBLEWINDOW_H
#define SCRIBBLEWINDOW_H

#include <QMainWindow>
#include "scribblearea.h"
#include "helper_functions.h"
namespace Ui {
    class ScribbleWindow;
}

class ScribbleWindow : public QMainWindow
{
        Q_OBJECT
        
    public:
        explicit ScribbleWindow(void* current_frame_ptr,QWidget *parent = nullptr);
        ~ScribbleWindow();
        
    private slots:
        void on_actionClean_Screen_triggered();
        
        void on_actionPen_Width_triggered();
        
        void on_actionPen_Color_triggered();
        
        void connect_actions();
        void save();
        bool save_file(const QByteArray &fileFormat);
        void on_actionback_triggered();
        
        void on_actionforward_triggered();
        
    private:
        Ui::ScribbleWindow *ui;
        ScribbleArea* scribble_area;
        cv::Mat current_frame;

};

#endif // SCRIBBLEWINDOW_H
