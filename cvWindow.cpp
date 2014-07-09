#include <cvWindow.h>

#include <mainwindow.h>
#include <QPainter>
#include <QTimer>
#include <QApplication>

cvWindow::cvWindow()
: _image(NULL), _ar_mode(Qt::KeepAspectRatio),
  _video_width(0), _video_height(0),
  _calibrationLimit(60), _averageFrequency(15),
  _correctionStep(true), _frameNumberForAverage(0),
  _alignmentLimit(2000), _height(0)
{
    setWindowTitle(tr("ezMove Demo"));
    resize(937, 937);
    
    _timer = new QTimer();

    recording = false;
    _patchDetection = new patchDetection();

    setWindowIcon(QIcon("logo.png"));

    speakerThread = new speaker("welcome");
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
        setCursor(Qt::PointingHandCursor);
        painter.fillRect(QRectF(QPoint(0, 0), QSize(width(), height())), Qt::white);
        painter.drawImage(QRect(1000/2 - 1000/2, 10, 937, 937), QImage("logo.png"));
        painter.drawText(0, 850, 937, 60, Qt::AlignHCenter, "Click to begin capture...");
        return;
    }

    QPainter video_painter(_image);
    video_painter.setRenderHints(QPainter::Antialiasing);

    std::vector<Patch> p = this->_patchDetection->getDetectedPatches();

    matchingPatches* _matchingPatches = matchingPatches::createIfMatching(p);

    if (_matchingPatches != NULL) {
        _frameNumberForAverage++;
        _patchHistory.push_back(_matchingPatches);
    } else {
        _frameNumberForAverage = 0;
        _patchHistory.clear();
    }

    if (_correctionStep) {

        if ( _frameNumberForAverage == _averageFrequency)
        {
            _frameNumberForAverage = 0;
            _patchHistory.clear();
        }

//        std::vector<Patch>::const_iterator second = p.begin(),end = p.end();

        //Draw lines between patches
//        std::sort(p.begin(), p.end(), Patch::compareByX);
//        if( second != end )
//        {
//           for(std::vector<Patch>::const_iterator first = second ++; second != end; ++first, ++second)
//            {
//                video_painter.setPen(QPen(QColor("#ff0080")));
//                QPoint r1(0,(*first).getRadius());
//                QPoint r2(0,(*second).getRadius());
//                QPoint p1 = (QPoint)*first += r1;
//                QPoint p2 = (QPoint)*second += r2;
//                QPoint p3 = (QPoint)*first -= r1;
//                QPoint p4 = (QPoint)*second -= r2;
//                video_painter.drawLine(p1, p2);
//                video_painter.drawLine(p3, p4);
//                video_painter.drawLine(*first, *second);
//           }
//        }

        if (_height != 0) {
            _drawHorizontalBar(video_painter, _matchingPatches);
        }

        _algo();
    } else {
        if ( _frameNumberForAverage == _calibrationLimit)
        {
            _frameNumberForAverage = 0;
            _height = _maxDistanceHeightInHistory();
            _patchHistory.clear();
            std::cout << "Found : " << _height << endl;
            _correctionStep = true;
        }

    }

    // draw patches anyway
    video_painter.setPen(QPen(Qt::black));
    std::vector<Patch>::const_iterator itr;
    for(itr = p.begin(); itr != p.end(); ++itr){
        _drawPatch(video_painter, (*itr));
    }


    // Draw a frame from the video
    _drawVideoFrame(painter);
    QWidget::paintEvent(e);
}

void cvWindow::_drawVideoFrame(QPainter& painter)
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

void cvWindow::_drawHorizontalBar(QPainter& painter, matchingPatches* p)
{
    painter.setPen(QPen(QColor(255, 255, 255, 255)));
    if (p == NULL) {
        painter.setBrush(QBrush(QColor(255, 255, 255, 128)));
    } else {
        int maxD = maxDistance(maxDistance(abs(p->getElbow().y() - _height), abs(p->getBow().y() - _height)),  abs(p->getCenter().y() - _height));

        if (maxD < _video_height/8) {
            painter.setBrush(QBrush(QColor(128, 255, 128, 128)));
        } else {
            painter.setBrush(QBrush(QColor(255, 255, 255, 128)));
        }
    }
    painter.drawRect(-5, _height - _video_height/16, _video_width + 10, _video_height/8);
}

void cvWindow::_drawPatch(QPainter& painter, Patch p)
{
    QColor crossColor;
    if ((_height != 0) && (abs(p.y() - _height) < 20)) {
        crossColor = QColor(128, 255, 128, 255);
    } else {
        crossColor = QColor(255, 127, 0);
    }

    painter.setPen(QPen(crossColor));
    painter.setBrush(QBrush(crossColor));
    painter.drawEllipse( p, p.getRadius(), p.getRadius());

    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse( p, 4*p.getRadius()/5, 4*p.getRadius()/5);

    int crossHeight = 5*p.getRadius()/6;
    int crossWidth = p.getRadius()/10;

    painter.setBrush(QBrush(crossColor));
    painter.drawRoundedRect(p.x() - crossHeight/2, p.y() - crossWidth/2, crossHeight, crossWidth, crossWidth/2, crossWidth/2);
    painter.drawRoundedRect(p.x() - crossWidth/2, p.y() - crossHeight/2, crossWidth, crossHeight, crossWidth/2, crossWidth/2);
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
        case Qt::Key_C:
        {
            _correctionStep = false;
            _frameNumberForAverage = 0;
            _patchHistory.clear();
            break;
        }
        case Qt::Key_H:
        {
            _height = _video_height / 2;
            break;
        }
        break;
    }
}

void cvWindow::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::LeftButton)
    {
        _start();
        return;
    }
    if(e->buttons() == Qt::RightButton) {
        _pause();
        return;
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

    setCursor(Qt::ArrowCursor);
}

void cvWindow::_close()
{
    emit closed();
}

int cvWindow::_maxDistanceHeightInHistory()
{
    int maxDistance = 0;
    int height = 0;
    std::vector<matchingPatches*>::const_iterator itr;
    for(itr = _patchHistory.begin(); itr != _patchHistory.end(); ++itr){
        int tmpDistance = abs((*itr)->getElbow().x() - (*itr)->getBow().x());
        if (maxDistance < tmpDistance) {
            maxDistance = tmpDistance;
            height = ((*itr)->getElbow().y() + (*itr)->getBow().y())/2;
        }
    }
    return height;
}
int cvWindow::maxDistance(int d1, int d2)
{
    if(d1 > d2)
        return d1;
    else
        return d2;
}

void cvWindow::_algo()
{
    if (_frameNumberForAverage == _averageFrequency - 1 )
    {
        if (_patchHistory.size() == _averageFrequency - 1 )
        {
            matchingPatches* averageMatchingPatches = matchingPatches::getAverage(_patchHistory);

            int distance_elbow_height = abs( averageMatchingPatches->getElbow().y() - _height);
            int distance_bow_height = abs( averageMatchingPatches->getBow().y() - _height);
            int distance_center_height = abs( averageMatchingPatches->getCenter().y() - _height);

            int _maxDistance = maxDistance(maxDistance(distance_elbow_height,distance_bow_height),distance_center_height);

            if( _maxDistance == distance_elbow_height)
            {
                switch(_height < averageMatchingPatches->getElbow().y())
                {
                    case 0:
                    {
                        speakerThread->setString("lower yuur elbow");
                        speakerThread->start();
                        break;
                    }
                    case 1:
                    {

                        speakerThread->setString("Raise yuur elbow");
                        speakerThread->start();
                        break;
                    }
                }
            }
            else if( _maxDistance == distance_bow_height)
            {
                switch(_height < averageMatchingPatches->getBow().y())
                {
                    case 0:
                    {

                        speakerThread->setString("lower yuur bow");
                        speakerThread->start();

                        break;
                    }
                    case 1:
                    {

                        speakerThread->setString("Raise yuur bow");
                        speakerThread->start();
                        break;
                    }
                }
            }
            else
            {
                switch(_height < averageMatchingPatches->getCenter().y())
                {
                    case 0:
                    {
                        speakerThread->setString("lower yuur hand");
                        speakerThread->start();
                        break;
                    }
                    case 1:
                    {
                        speakerThread->setString("Raise yuur hand");
                        speakerThread->start();
                        break;
                    }
                }
            }
        }
    }
}
