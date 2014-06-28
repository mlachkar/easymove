TEMPLATE = app

QT += core gui widgets

SOURCES += main.cpp \
    cvWindow.cpp \
    patch.cpp \
    patchDetection.cpp

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    cvWindow.h \
    patch.h \
    patchDetection.h

## OpenCV settings for Unix/Linux
unix:!mac {
    INCLUDEPATH += /usr/include/opencv

    LIBS += -L/usr/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lcvblob
}

OTHER_FILES +=
