#pragma once
#include <highgui.h>
#include <cv.h>
#include <cvblob.h>
#include "patch.h"

using namespace cvb;

class patchDetection
{
public:
    patchDetection();
    //~patchDetection();
    
    float getRedLimit() const;
    float getBlueLimit() const;
    
    int getWidth() const;
    int getHeight() const;
    
    void setRedLimit(float);
    void setBlueLimit(float);
    
    IplImage* getFrame(bool displayBlobs);
    std::vector<Patch> getDetectedPatches();

private:
    float _redLimit;
    float _blueLimit;
    CvSize _imgSize;
    IplImage* _frame;
    CvCapture* _capture;
    void grabFrame();
    std::vector<Patch> _detectedPatches;
};
