#pragma once
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QImage>
#include <QKeyEvent>

#include <patchDetection.h>
#include <speaker.h>
#include <matchingpatches.h>

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

    //void cvWindow::_maxDistance(Patch p1, Patch p2);

    //void cvWindow::_algoCalibration(matchingPatches _matchingPatches);
    void _algo(QPoint ref);

private:
    QImage* _image;
    Qt::AspectRatioMode _ar_mode;
    int _video_width;
    int _video_height;
    QTimer* _timer;
    patchDetection* _patchDetection;
    std::vector<matchingPatches*> _patchHistory;
    speaker* _speaker;

    unsigned char _averageFrequency;
    unsigned char _frameNumberForAverage;
    unsigned int _alignmentLimit;
    bool recording;
    CvVideoWriter* writer ;

private slots:
    void _tick();
    void _start();
    void _pause();
//    void _recVideo();

protected:
    void _close();

signals:
    void closed();
};
