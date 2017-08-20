#-------------------------------------------------
#
# Project created by QtCreator 2017-02-10T21:13:51
#
#-------------------------------------------------

QT       += core gui sql
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OMR
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

#-unix: CONFIG += link_pkgconfig
#-unix: PKGCONFIG += opencv

SOURCES += main.cpp\
        mainwindow.cpp \
    answersheet.cpp \
    createform.cpp \
    omrprocess.cpp \
    clickablelabel.cpp

HEADERS  += mainwindow.h \
    answersheet.h \
    createform.h \
    asmOpenCV.h \
    omrprocess.h \
    clickablelabel.h

FORMS    += mainwindow.ui \
    createform.ui \
    omrprocess.ui
INCLUDEPATH += C:\opencv\opencv\build\include

LIBS += C:\opencv\opencv_bin\bin\libopencv_core2413.dll
LIBS += C:\opencv\opencv_bin\bin\libopencv_highgui2413.dll
LIBS += C:\opencv\opencv_bin\bin\libopencv_imgproc2413.dll
LIBS += C:\opencv\opencv_bin\bin\libopencv_features2d2413.dll
LIBS += C:\opencv\opencv_bin\bin\libopencv_calib3d2413.dll


include(QZXing/QZXing.pri)
