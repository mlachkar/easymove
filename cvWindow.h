#pragma once
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QImage>
#include <QKeyEvent>

#include "patchDetection.h"

class cvWindow : public QMainWindow
{
    Q_OBJECT

public:
    cvWindow();
    ~cvWindow();

    void _draw_video_frame(QPainter& painter, bool grayed);
    void _draw_info_text(QPainter& painter);
    void _draw_patch(QPainter& painter, Patch p);


    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent* event);
    
    unsigned char getAverageFrequency() const;
    void setAverageFrequency(unsigned char);

private:
    QMenuBar _menu_bar;
    QMenu* _menu;
    QImage* _image;
    int _tick_ms;
    Qt::AspectRatioMode _ar_mode;
    int _fps;
    int _video_width;
    int _video_height;
    QTimer* _timer;
    patchDetection* _patchDetection;
    
    unsigned char _averageFrequency;
    unsigned char _frameNumberForAverage;
    char* _alignementValues;
    unsigned char _getAlignmentAverage();
    unsigned int _alignmentLimit;
    bool _grayed;

private slots:
    //void _open();
    void _tick();
    void _start();

protected:
    void _close();

signals:
    void closed();
};
