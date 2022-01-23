#ifndef SCRIBBLEWINDOW_H
#define SCRIBBLEWINDOW_H

#include <QMainWindow>
#include <atomic>
#include "scribblearea.h"
#include "helper_functions.h"

extern atomic<int> active_scribble_window_nb;

namespace Ui {
    class ScribbleWindow;
}

class ScribbleWindow : public QMainWindow
{
        Q_OBJECT
        
    public:
        explicit ScribbleWindow(int index, QWidget *parent = nullptr);
        
        ~ScribbleWindow();
        
        void closeEvent(QCloseEvent *event);
        void set_pars(void* current_frame_ptr);
        
        int index;
        bool is_active;        
    private slots:
        void on_actionClean_Screen_triggered();
        
        void on_actionPen_Width_triggered();
        
        void on_actionPen_Color_triggered();
        
        void connect_actions();
        void save();
        bool save_file(const QByteArray &fileFormat);
        void on_actionback_triggered();
        
        void on_actionforward_triggered();
        
        void on_actionEraser_triggered();
        
        void on_actionEraser_Size_triggered();
        
    private:
        Ui::ScribbleWindow *ui;
        ScribbleArea* scribble_area = nullptr;
        cv::Mat current_frame;
};

#endif // SCRIBBLEWINDOW_H
