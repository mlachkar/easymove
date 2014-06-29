#pragma once
#include <highgui.h>
#include <cv.h>
#include <cvblob.h>
#include <QImage>
#include <patch.h>

using namespace cvb;

class patchDetection
{
public:
    patchDetection();
    ~patchDetection();
    
    float getRedLimit() const;
    float getBlueLimit() const;
    
    unsigned int getMinBlobArea() const;
    unsigned int getMaxBlobArea() const;

    int getWidth() const;
    int getHeight() const;
    
    void setRedLimit(float);
    void setBlueLimit(float);

    void setMinBlobArea(unsigned int);
    void setMaxBlobArea(unsigned int);

    IplImage* getFrame();
    std::vector<Patch> getDetectedPatches();

    void detectPatches(bool displayBlobs);

private:
    float _redLimit;
    float _blueLimit;
    unsigned int _minBlobArea;
    unsigned int _maxBlobArea;
    CvSize _imgSize;
    IplImage* _frame;
    CvCapture* _capture;
    void grabFrame();
    std::vector<Patch> _detectedPatches;
};
