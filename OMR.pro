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

unix {
unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv
}
win32 {
INCLUDEPATH += include

LIBS += D:\Develop\QT\OMR\lib\libopencv_core2413.dll
LIBS += D:\Develop\QT\OMR\lib\libopencv_highgui2413.dll
LIBS += D:\Develop\QT\OMR\lib\libopencv_imgproc2413.dll
}


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




include(QZXing/QZXing.pri)
