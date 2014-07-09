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

    void _drawVideoFrame(QPainter& painter);
    void _drawPatch(QPainter& painter, Patch p);
    void _drawHorizontalBar(QPainter& painter, matchingPatches* p);

    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent *e);
    
    unsigned char getAverageFrequency() const;
    void setAverageFrequency(unsigned char);

private:
    QImage* _image;
    Qt::AspectRatioMode _ar_mode;
    int _video_width;
    int _video_height;
    QTimer* _timer;
    patchDetection* _patchDetection;
    std::vector<matchingPatches*> _patchHistory;

    unsigned char _calibrationLimit;
    unsigned char _averageFrequency;
    bool _correctionStep;
    unsigned char _frameNumberForAverage;
    unsigned int _alignmentLimit;
    bool recording;
    CvVideoWriter* writer;

    int maxDistance(int d1, int d2);
    int _height;
    speaker* speakerThread ;
    int _maxDistanceHeightInHistory();
    void _algo();

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
