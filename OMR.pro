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

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv




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

LIBS +=   -lboost_system \
-lboost_filesystem \
#-lzxing \
-lboost_serialization

include(QZXing/QZXing.pri)
