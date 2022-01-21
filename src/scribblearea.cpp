/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QMainWindow>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif
#include "scribblearea.h"

#include <deque>
#include <iostream>


using namespace std;
//! [0]
ScribbleArea::ScribbleArea(QImage img,QWidget *parent)
    : QWidget(parent)
{
    image = img;
    image_original = img;
    setAttribute(Qt::WA_StaticContents);
    modified = false;
    scribbling = false;
    changed_pixelsQ_index = 0;
    myPenWidth = 1;
    myPenColor = Qt::blue;
    this->openImage();
}
//! [0]

//! [1]
bool ScribbleArea::openImage(){

    QSize newSize = this->image.size().expandedTo(size());
    resizeImage(&this->image, newSize);
    modified = false;
    update();
    return true;
}
bool ScribbleArea::saveImage(const QString &fileName, const char *fileFormat){
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}
void ScribbleArea::setPenColor(const QColor &newColor){
    myPenColor = newColor;
}
void ScribbleArea::setPenWidth(int newWidth){
    myPenWidth = newWidth;
}
void ScribbleArea::clearImage(){
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}
void ScribbleArea::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton) {
        cout<<"press event"<<endl;
        lastPoint = event->pos();
        scribbling = true;
    }
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event){
    if ((event->buttons() & Qt::LeftButton) && scribbling){
        drawLineTo(event->pos());
       // cout<<"move event"<<endl;
    }
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton && scribbling) {
        //cout<<"release event"<<endl;
        scribbling = false;
    }
}

void ScribbleArea::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}
void ScribbleArea::resizeEvent(QResizeEvent *event){
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}
void ScribbleArea::drawLineTo(const QPoint &endPoint){
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    modified = true;

    int rad = (myPenWidth / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
  
    //changed_pixels.push_back(lastPoint);
    if(changed_pixelQ.size()>=MAX_BACK_MOVE_NB){    
        changed_pixelQ.pop_front();
    }
    changed_pixel temp_px;
    temp_px.pix_pos = endPoint;
    temp_px.old_color = image.pixelColor(endPoint.x(),endPoint.y());
    temp_px.new_color = this->penColor();
    while(changed_pixelQ.size()-changed_pixelsQ_index>=2){
        changed_pixelQ.pop_front();
    }
    cout<<"pos x : "<<endPoint.x()<<" pos y : "<<endPoint.y()<<endl;
    changed_pixelQ.push_back(temp_px);
    changed_pixelsQ_index = changed_pixelQ.size()-1;
    lastPoint = endPoint;
}
void ScribbleArea::resizeImage(QImage *image, const QSize &newSize){
    if (image->size() == newSize){
        return;
    }

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void ScribbleArea::print(){
//#if QT_CONFIG(printdialog)
//    QPrinter printer(QPrinter::HighResolution);
//
//    QPrintDialog printDialog(&printer, this);
////! [21] //! [22]
//    if (printDialog.exec() == QDialog::Accepted) {
//        QPainter painter(&printer);
//        QRect rect = painter.viewport();
//        QSize size = image.size();
//        size.scale(rect.size(), Qt::KeepAspectRatio);
//        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
//        painter.setWindow(image.rect());
//        painter.drawImage(0, 0, image);
//    }
    //#endif // QT_CONFIG(printdialog)
}

void ScribbleArea::go_back(){
    changed_pixel temp_pix = changed_pixelQ.at(changed_pixelsQ_index);
    image.setPixelColor(temp_pix.pix_pos,temp_pix.old_color);
    changed_pixelsQ_index--;
    
    painter.drawLine(temp_pix.pix_pos, temp_pix.pix_pos);
    //update(QRect(temp_pix.pix_pos, temp_pix.pix_pos).normalized().adjusted(-rad, -rad, +rad, +rad));
    update();
}

void ScribbleArea::go_forward(){
    if(changed_pixelsQ_index < changed_pixelQ.size()-1){
        changed_pixel temp_pix = changed_pixelQ.at(changed_pixelsQ_index);
        this->image.setPixelColor(temp_pix.pix_pos,temp_pix.new_color);
        
        painter.drawLine(temp_pix.pix_pos, temp_pix.pix_pos);
        changed_pixelsQ_index++;
       // update(QRect(temp_pix.pix_pos, temp_pix.pix_pos).normalized().adjusted(-rad, -rad, +rad, +rad));
        
    }
    
}
