#include "cvWindow.h"

#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QTimer>
#include <QApplication>

cvWindow::cvWindow()
: _menu(NULL), _image(NULL), _tick_ms(33), _ar_mode(Qt::KeepAspectRatio),
  _fps(0), _video_width(0), _video_height(0), _timer(NULL), 
  _averageFrequency(15), _alignmentLimit(2000)
{
    setWindowTitle(tr("EasyMove Demo"));
    resize(480, 240);
    
    this->_alignementValues = new char[this->_averageFrequency];

    _menu = new QMenu("File");
    _menu->addAction("Start", this, SLOT(_start()));
    _menu->addSeparator();
    _menu->addAction("Exit", this, SLOT(close()));
    _menu_bar.addMenu(_menu);

    _patchDetection = new patchDetection();
	setMenuBar(&_menu_bar);
}

cvWindow::~cvWindow()
{
    if (_timer)
    {
        _timer->stop();
        delete _timer;
    }

    if (_menu)
        delete _menu;

    if (_image)
        delete _image;

    //if (_capture)
    {
        //delete _capture;
    }
}

void cvWindow::_tick()
{
    cv::Mat frame(this->_patchDetection->getFrame(false));
/*
    if (p.size() > 100) {
        std::sort(p.begin(), p.end(), Patch::yX);
        
        bool horizontal = p[2].getY() - p[0].getY() < 40;    

        std::sort(p.begin(), p.end(), Patch::yY);
          
        // Produit scalaire des deux vecteurs entre les 3 points
        unsigned int alignedScoreRaw = abs((p[0].getX() - p[1].getY()) * (p[1].getY() - p[2].getY()) - (p[0].getY() - p[1].getY()) * (p[1].getX() - p[2].getX()));
          
        this->_alignementValues[this->_frameNumberForAverage] = alignedScoreRaw < this->_alignmentLimit ? 100 - alignedScoreRaw * 100 / this->_alignmentLimit : 0;
        bool aligned = this->_alignementValues[this->_frameNumberForAverage] > 0;
          
        this->_frameNumberForAverage++;
          
        if (this->_frameNumberForAverage == this->_averageFrequency) {
            std::cout << static_cast<unsigned>(this->_getAlignmentAverage()) << "%" << std::endl;
            this->_frameNumberForAverage = 0;
            //cv::Mat* mtx = new cv::Mat(frame);
            //cv::putText(*mtx, static_cast<unsigned>(average(alignement_values)), cv::Point(100, 100), CV_FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255),1,8,false);
            //cv::CvFont font = cv::fontQt("Times");
            //addText( frame, "Hello World !", Point(50,50), font);
        }
          
        CvScalar const color = aligned ? CV_RGB(0, 255, 0) : CV_RGB(255, 0, 0);
        
        if (horizontal && aligned) {
            cvLine(this->_frame, cvPoint(p[0].getX(), p[0].getY() + 10), cvPoint(p[2].getX(), p[2].getY() + 10), color);
            cvLine(this->_frame, cvPoint(p[0].getX(), p[0].getY() - 10), cvPoint(p[2].getX(), p[2].getY() - 10), color);
            
            if (p[0].getColor() == BLUE) {
                cvLine(this->_frame, cvPoint(p[0].getX() - 10, p[0].getY()), cvPoint(p[0].getX() + 10, p[0].getY() + 20), color);
                cvLine(this->_frame, cvPoint(p[0].getX() - 10, p[0].getY()), cvPoint(p[0].getX() + 10, p[0].getY() - 20), color);
            } else {
                cvLine(this->_frame, cvPoint(p[2].getX() + 10, p[2].getY()), cvPoint(p[2].getX() - 10, p[2].getY() + 20), color);
                cvLine(this->_frame, cvPoint(p[2].getX() + 10, p[2].getY()), cvPoint(p[2].getX() - 10, p[2].getY() - 20), color);
            }
        } else {
            cvLine(this->_frame, cvPoint(p[0].getX(), p[0].getY()), cvPoint(p[1].getX(), p[1].getY()), color);
            cvLine(this->_frame, cvPoint(p[1].getX(), p[1].getY()), cvPoint(p[2].getX(), p[2].getY()), color);
        }
        
    } else {
      this->_frameNumberForAverage = 0;
    }*/
    

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

    // Draw a frame from the video
    _draw_video_frame(painter, this->_grayed);

    // Draw text with info about the video
    //_draw_info_text(painter);
    
    std::vector<Patch> p = this->_patchDetection->getDetectedPatches();

    std::vector<Patch>::const_iterator itr;
    for(itr = p.begin(); itr != p.end(); ++itr){        
        _draw_patch(painter, (*itr));
    }
    std::vector<Patch>::const_iterator itr2;
    std::sort(p.begin(), p.end(), Patch::compareByX);
//    Patch prev = *p.begin();
//    for(itr2 = p.begin(); itr2 != p.end(); ++itr2){
//        _draw_line(painter, prev, (*itr2));
//        prev = (*itr2);
//    }

    QWidget::paintEvent(e);
}

void cvWindow::_draw_video_frame(QPainter& painter, bool grayed)
{
    if (grayed) {
        painter.setBrush(QBrush(QColor(255, 255, 255, 128)));
    }
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

void cvWindow::_draw_line(QPainter& painter, Patch p1, Patch p2 )
{
    painter.drawLine(p1, p2);
}
void cvWindow::_draw_info_text(QPainter& painter)
{
    // Setup font properties
    painter.setFont(QFont("Arial", 16));
    painter.setPen(QPen(QColor("#2e8b57")));

    // Draw info about FPS
    QString fps = "Moyenne sur " + QString::number(_fps) + " frames";
    painter.drawText(QRect(10, 22, 100, 100), Qt::AlignLeft, fps);
}

void cvWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        // ESC: exit application
        case Qt::Key_Escape:
        {
            close();
        }
        case Qt::Key_S:
        {
            _start();
        }
        break;
    }
}

void cvWindow::_start()
{
    _video_width = _patchDetection->getWidth();
    _video_height = _patchDetection->getHeight();
    
    // Resize the window to fit video dimensions
    resize(_video_width, _video_height);

    // Retrieve fps from the video. If not available, default will be 25
    _fps = this->_averageFrequency;

    // Set FPS playback
    _tick_ms = 1000/_fps;

    _image = new QImage(_video_width, _video_height, QImage::Format_RGB888);

    // Start timer to read frames from the capture interface
    _timer = new QTimer();
    _timer->start(_tick_ms);
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}

void cvWindow::_close()
{
    qDebug() << "cvWindow::_close";
    emit closed();
}
    
unsigned char cvWindow::_getAlignmentAverage() {
  unsigned int sum = 0;
  for (int i=0;i<this->_averageFrequency;i++) {
    sum += this->_alignementValues[i];
  }
  return sum/this->_averageFrequency;
}
