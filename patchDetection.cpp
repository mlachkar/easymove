#include "patchDetection.h"

patchDetection::patchDetection()
: _redLimit(0.2), _blueLimit(0.2)
{
    this->_capture = cvCaptureFromCAM(0);
    this->grabFrame();
    IplImage* img = cvRetrieveFrame(this->_capture);
    this->_imgSize = cvGetSize(img);
    this->_frame = cvCreateImage(this->_imgSize, img->depth, img->nChannels);
}

float patchDetection::getRedLimit() const {
  return this->_redLimit;
}
float patchDetection::getBlueLimit() const {
  return this->_blueLimit;
}
int patchDetection::getWidth() const {
  return this->_imgSize.width;
}
int patchDetection::getHeight() const {
  return this->_imgSize.height;
}
std::vector<Patch> patchDetection::getDetectedPatches() {
  return this->_detectedPatches;
}

void patchDetection::setRedLimit(float redLimit) {
  this->_redLimit = redLimit;
}
void patchDetection::setBlueLimit(float blueLimit) {
  this->_blueLimit = blueLimit;
}

IplImage* patchDetection::getFrame(bool displayBlobs) {
  
    this->grabFrame();
    IplImage* img = cvRetrieveFrame(this->_capture);
    cvFlip (img, img, 1);

    cvConvertScale(img, this->_frame, 1, 0);

    IplImage *segmentatedRed = cvCreateImage(this->_imgSize, 8, 1);
    IplImage *segmentatedBlue = cvCreateImage(this->_imgSize, 8, 1);
    
    // Detecting red pixels:
    // (This is very slow, use direct access better...)
    for (int j=0; j<this->_imgSize.height; j++)
      for (int i=0; i<this->_imgSize.width;i++)
      {
	      CvScalar c = cvGet2D(this->_frame, j, i);

	      double b = ((double)c.val[0])/255.;
	      double g = ((double)c.val[1])/255.;
	      double r = ((double)c.val[2])/255.;
	      unsigned char f_red = 255*((r> this->_redLimit +g)&&(r> this->_redLimit +b));
	      unsigned char f_blue = 255*((b> this->_blueLimit +g)&&(b> this->_blueLimit +r));

	      cvSet2D(segmentatedRed, j, i, CV_RGB(f_red, f_red, f_red));
	      cvSet2D(segmentatedBlue, j, i, CV_RGB(f_blue, f_blue, f_blue));
      }

    IplImage *labelImg = cvCreateImage(cvGetSize(this->_frame), IPL_DEPTH_LABEL, 1);

    CvBlobs redBlobs, blueBlobs;
    cvLabel(segmentatedRed, labelImg, redBlobs);
    cvLabel(segmentatedBlue, labelImg, blueBlobs);

    cvFilterByArea(redBlobs, 500, 1000000);
    cvFilterByArea(blueBlobs, 500, 1000000);
    
    if (displayBlobs) {
      cvRenderBlobs(labelImg, redBlobs, this->_frame, this->_frame, CV_BLOB_RENDER_BOUNDING_BOX);
      cvRenderBlobs(labelImg, blueBlobs, this->_frame, this->_frame, CV_BLOB_RENDER_BOUNDING_BOX);
    }

    this->_detectedPatches.clear();

    // we only consider the 3 points case
    //if (blueBlobs.size() == 1 && redBlobs.size() == 2) {  

    std::map<CvLabel, CvBlob * >::const_iterator itr;  

    for(itr = blueBlobs.begin(); itr != blueBlobs.end(); ++itr){     
        Patch patch((*itr).second->centroid.x, (*itr).second->centroid.y, (*itr).second->minx, (*itr).second->miny, (*itr).second->maxx, (*itr).second->maxx, BLUE);
        this->_detectedPatches.push_back(patch);
    }
      
    for(itr = redBlobs.begin(); itr != redBlobs.end(); ++itr){        
        Patch patch((*itr).second->centroid.x, (*itr).second->centroid.y, (*itr).second->minx, (*itr).second->miny, (*itr).second->maxx, (*itr).second->maxx, RED);
        this->_detectedPatches.push_back(patch);
    } 
    //} 

    cvReleaseImage(&labelImg);
    cvReleaseImage(&segmentatedRed);
    cvReleaseImage(&segmentatedBlue);

    cvReleaseBlobs(redBlobs);
    cvReleaseBlobs(blueBlobs);

    return this->_frame;
}

void patchDetection::grabFrame() {
  cvGrabFrame(this->_capture);
}

