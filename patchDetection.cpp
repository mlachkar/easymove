#include "patchDetection.h"

patchDetection::patchDetection()
: _redLimit(0.2), _blueLimit(0.2), _minBlobArea(500), _maxBlobArea(1000000)
{
    _capture = cvCaptureFromCAM(0);
    cvGrabFrame(_capture);
    IplImage* img = cvRetrieveFrame(_capture);
    _imgSize = cvGetSize(img);
    _frame = cvCreateImage(_imgSize, img->depth, img->nChannels);
}

float patchDetection::getRedLimit() const {
  return _redLimit;
}

float patchDetection::getBlueLimit() const {
  return _blueLimit;
}

unsigned int patchDetection::getMinBlobArea() const {
  return _minBlobArea;
}

unsigned int patchDetection::getMaxBlobArea() const {
  return _maxBlobArea;
}

int patchDetection::getWidth() const {
  return _imgSize.width;
}

int patchDetection::getHeight() const {
  return _imgSize.height;
}

std::vector<Patch> patchDetection::getDetectedPatches() {
  return _detectedPatches;
}

void patchDetection::setRedLimit(float redLimit) {
  _redLimit = redLimit;
}
void patchDetection::setBlueLimit(float blueLimit) {
  _blueLimit = blueLimit;
}
void patchDetection::setMinBlobArea(unsigned int minBlobArea) {
  _minBlobArea = minBlobArea;
}
void patchDetection::setMaxBlobArea(unsigned int maxBlobArea) {
  _maxBlobArea = maxBlobArea;
}

IplImage* patchDetection::getFrame() {
    return _frame;
}

void patchDetection::detectPatches(bool displayBlobs) {
    cvGrabFrame(_capture);
    IplImage* img = cvRetrieveFrame(_capture);
    cvFlip (img, img, 1);

    cvConvertScale(img, _frame, 1, 0);

    IplImage *segmentatedRed = cvCreateImage(_imgSize, 8, 1);
    IplImage *segmentatedBlue = cvCreateImage(_imgSize, 8, 1);
    
    // Detecting red pixels:
    // (This is very slow, use direct access better...)
    for (int j=0; j<_imgSize.height; j++)
      for (int i=0; i<_imgSize.width;i++)
      {
          CvScalar c = cvGet2D(_frame, j, i);

	      double b = ((double)c.val[0])/255.;
	      double g = ((double)c.val[1])/255.;
	      double r = ((double)c.val[2])/255.;
          unsigned char f_red = 255*((r> _redLimit +g)&&(r> _redLimit +b));
          unsigned char f_blue = 255*((b> _blueLimit +g)&&(b> _blueLimit +r));

	      cvSet2D(segmentatedRed, j, i, CV_RGB(f_red, f_red, f_red));
	      cvSet2D(segmentatedBlue, j, i, CV_RGB(f_blue, f_blue, f_blue));
      }

    IplImage *labelImg = cvCreateImage(cvGetSize(_frame), IPL_DEPTH_LABEL, 1);

    CvBlobs redBlobs, blueBlobs;
    cvLabel(segmentatedRed, labelImg, redBlobs);
    cvLabel(segmentatedBlue, labelImg, blueBlobs);

    cvFilterByArea(redBlobs, _minBlobArea, _maxBlobArea);
    cvFilterByArea(blueBlobs, _minBlobArea, _maxBlobArea);
    
    if (displayBlobs) {
      cvRenderBlobs(labelImg, redBlobs, _frame, _frame, CV_BLOB_RENDER_BOUNDING_BOX);
      cvRenderBlobs(labelImg, blueBlobs, _frame, _frame, CV_BLOB_RENDER_BOUNDING_BOX);
    }

    _detectedPatches.clear();

    std::map<CvLabel, CvBlob * >::const_iterator itr;  

    for(itr = blueBlobs.begin(); itr != blueBlobs.end(); ++itr){     
        Patch patch((*itr).second->centroid.x, (*itr).second->centroid.y, (*itr).second->minx, (*itr).second->miny, (*itr).second->maxx, (*itr).second->maxx, BLUE);
        _detectedPatches.push_back(patch);
    }
      
    for(itr = redBlobs.begin(); itr != redBlobs.end(); ++itr){        
        Patch patch((*itr).second->centroid.x, (*itr).second->centroid.y, (*itr).second->minx, (*itr).second->miny, (*itr).second->maxx, (*itr).second->maxx, RED);
        _detectedPatches.push_back(patch);
    }

    cvReleaseImage(&labelImg);
    cvReleaseImage(&segmentatedRed);
    cvReleaseImage(&segmentatedBlue);

    cvReleaseBlobs(redBlobs);
    cvReleaseBlobs(blueBlobs);
}

