TEMPLATE = app

QT += core gui widgets

SOURCES += main.cpp \
    cvWindow.cpp \
    patch.cpp \
    patchDetection.cpp \
    speaker.cpp \
    matchingpatches.cpp \
    mainwindow.cpp \
    interfacereglages.cpp

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    cvWindow.h \
    patch.h \
    patchDetection.h \
    speaker.h \
    matchingpatches.h \
    mainwindow.h \
    interfacereglages.h

## OpenCV settings for Unix/Linux
unix:!mac {
    INCLUDEPATH += /usr/include/opencv

    LIBS += -L/usr/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lcvblob \
        -lFestival \
        -lestools \
        -lestbase \
        -leststring
}

ICON = logo.png

OTHER_FILES += \
    logo.png

FORMS += \
    mainwindow.ui
