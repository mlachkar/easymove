#pragma once
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QImage>
#include <QKeyEvent>

#include <patchDetection.h>
#include <speaker.h>

class cvWindow : public QWidget
{
    Q_OBJECT

public:
    cvWindow();
    ~cvWindow();

    void _draw_video_frame(QPainter& painter);
    void _draw_patch(QPainter& painter, Patch p);

    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent* event);
    
    unsigned char getAverageFrequency() const;
    void setAverageFrequency(unsigned char);

    void _algo();
private:
    QImage* _image;
    Qt::AspectRatioMode _ar_mode;
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
    void _tick();
    void _start();
    void _pause();

protected:
    void _close();

signals:
    void closed();
};
