#include <cvWindow.h>

#include <QPainter>
#include <QTimer>
#include <QApplication>

cvWindow::cvWindow()
: _image(NULL), _ar_mode(Qt::KeepAspectRatio),
  _video_width(0), _video_height(0),
  _averageFrequency(15), _alignmentLimit(2000)
{
    setWindowTitle(tr("EasyMove Demo"));
    resize(480, 240);
    
    _timer = new QTimer();
    this->_alignementValues = new char[this->_averageFrequency];

    recording = false;
    _patchDetection = new patchDetection();

}

cvWindow::~cvWindow()
{
    if (_timer)
    {
        _timer->stop();
        delete _timer;
    }

    if (_image)
        delete _image;

    if (_patchDetection)
        delete _patchDetection;
}

void cvWindow::_tick()
{
    _patchDetection->detectPatches(false);
    cv::Mat frame(this->_patchDetection->getFrame());

    // Since OpenCV uses BGR order, we need to convert it to RGB
    cv::cvtColor(frame, frame, CV_BGR2RGB);

    // Copy cv::Mat to QImage
    memcpy(_image->scanLine(0), (unsigned char*)frame.data, _image->width() * _image->height() * frame.channels());

    // Trigger paint event to redraw the window
    update();
}

void cvWindow::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);

    // When no image is loaded, paint the window with black
    if (!_image)
    {
        painter.fillRect(QRectF(QPoint(0, 0), QSize(width(), height())), Qt::black);
        QWidget::paintEvent(e);
        return;
    }

    QPainter video_painter(_image);
    video_painter.setRenderHints(QPainter::Antialiasing);

    std::vector<Patch> p = this->_patchDetection->getDetectedPatches();

    std::vector<Patch>::const_iterator second = p.begin(),end = p.end();

    //Draw lines between patches
    std::sort(p.begin(), p.end(), Patch::compareByX);
    if( second != end )
    {
       for(std::vector<Patch>::const_iterator first = second ++; second != end; ++first, ++second)
        {
            video_painter.setPen(QPen(QColor("#ff0080")));
            QPoint r1(0,(*first).getRadius());
            QPoint r2(0,(*second).getRadius());
            QPoint p1 = (QPoint)*first += r1;
            QPoint p2 = (QPoint)*second += r2;
            QPoint p3 = (QPoint)*first -= r1;
            QPoint p4 = (QPoint)*second -= r2;
            video_painter.drawLine(p1, p2);
            video_painter.drawLine(p3, p4);
            video_painter.drawLine(*first, *second);
       }
    }

    video_painter.setPen(QPen(Qt::black));
    std::vector<Patch>::const_iterator itr;
    for(itr = p.begin(); itr != p.end(); ++itr){
        _draw_patch(video_painter, (*itr));
        _algo();
    }


    // Draw a frame from the video
    _draw_video_frame(painter);

    QWidget::paintEvent(e);
}

void cvWindow::_draw_video_frame(QPainter& painter)
{
    switch (_ar_mode)
    {
        default:
        case Qt::IgnoreAspectRatio:
        {
            painter.drawImage(QRectF(QPoint(0, 0), QSize(width(), height())), *_image, QRectF(QPoint(0, 0), _image->size()));
            
        }
        break;

        case Qt::KeepAspectRatio:
        {
            painter.fillRect(QRectF(QPoint(0, 0), QSize(width(), height())), Qt::black);

            QImage scaled_img = _image->scaled(QSize(width(), height()), Qt::KeepAspectRatio, Qt::FastTransformation);

            painter.drawImage(qRound(width()/2.0)  - qRound(scaled_img.size().width()/2.0),
                              qRound(height()/2.0) - qRound(scaled_img.size().height()/2.0),
                              scaled_img);
        }
        break;

        case Qt::KeepAspectRatioByExpanding:
        {
            QImage scaled_img = _image->scaled(QSize(width(), height()), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);

            painter.drawImage(QRectF(QPoint(0, 0), QSize(width(), height())),
                               scaled_img,
                               QRectF(QPoint(qRound(scaled_img.size().width()/2.0) - qRound(width()/2.0),
                                             qRound(scaled_img.size().height()/2.0) - qRound(height()/2.0)),
                                             QSize(width(), height())));
        }
        break;
    }
}

void cvWindow::_draw_patch(QPainter& painter, Patch p)
{
    if (p.getColor() == BLUE) {
        painter.setBrush(QBrush(QColor(128, 128, 255, 128)));
    } else {
        painter.setBrush(QBrush(QColor(255, 128, 128, 128)));
    }
    painter.drawEllipse( p, p.getRadius(), p.getRadius());
}

void cvWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        // ESC: exit application
        case Qt::Key_Escape:
        case Qt::Key_Q:
        {
            close();
        }
        case Qt::Key_P:
        {
            _pause();
            break;
        }
        case Qt::Key_S:
        {
            _start();
            break;
        }
        case Qt::Key_R:
        {
            _recVideo();
            break;
        }
        break;
    }
}

void cvWindow::_pause()
{
    if (_timer->isActive()) {
        _timer->stop();
    } else {
        _timer->start();
    }
}

void cvWindow::_recVideo()
{
    if (recording){

        cvGrabFrame(_patchDetection->getCapture());
        IplImage* img = cvRetrieveFrame(_patchDetection->getCapture());
        writer = cvCreateVideoWriter("/home/ezmove/MonEnregistrement.avi",-1,20,cvSize(img->width,img->height),1);
        cvWriteFrame(writer,img);
     }
    while(1) {

     frames = cvQueryFrame(_patchDetection->getCapture());
     if( !frames ) break;
     cvShowImage( "Enregister ..... Appuyer sur Echap pour sortir !", frames );
     cvWriteFrame( writer, frames );
    }
    cvReleaseVideoWriter( &writer );
}


void cvWindow::_start()
{
    if (_timer->isActive()) {
        return;
    }
    _video_width = _patchDetection->getWidth();
    _video_height = _patchDetection->getHeight();
    
    // Resize the window to fit video dimensions
    resize(_video_width, _video_height);

    _image = new QImage(_video_width, _video_height, QImage::Format_RGB888);

    // Start timer to read frames from the capture interface
    _timer->start(1000/_averageFrequency);
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(_tick()));

    recording = true;
}

void cvWindow::_close()
{
    emit closed();
}
    
unsigned char cvWindow::_getAlignmentAverage() {
  unsigned int sum = 0;
  for (int i=0;i<this->_averageFrequency;i++) {
    sum += this->_alignementValues[i];
  }
  return sum/this->_averageFrequency;
}
void cvWindow::_algo()
{
    std::vector<Patch> p = this->_patchDetection->getDetectedPatches();
    std::sort(p.begin(), p.end(), Patch::compareByX);
    std::vector<Patch> pBlue;
    std::vector<Patch> pRed;
    speaker *_speaker = new speaker();

    std::vector<Patch>::const_iterator itr;
    for(itr = p.begin(); itr != p.end(); ++itr)
    {
        if((*itr).getColor() == BLUE)
        {
            pBlue.push_back(*itr);
        }
        else
        {   pRed.push_back(*itr);
//            std::cout << "pRed.size()" << pRed.size();
//            std::cout <<""<< std::endl;
        }
    }

    QPoint ref(100,100);

    if(pRed.size()==1 && pBlue.size() == 0) // test avec un seul patch
    {
        if(p[0].getColor() == RED)
        {
            if (p[0].y() >= ref.y()){

                _speaker->say("ok"); // Il faudrait faire une moyenne, sinon le speaker n'a pas le temps de parler pendant une frame
            }
            std::cout << "Que la correction commence !" << std::endl;

        }

    }
}

