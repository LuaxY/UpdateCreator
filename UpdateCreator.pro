#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T10:31:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UpdateCreator
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        updatecreator.cpp

HEADERS  += updatecreator.h

FORMS    += updatecreator.ui

LIBS += -L$$PWD/AWS/aws-cpp-sdk-core/ -laws-cpp-sdk-core
LIBS += -L$$PWD/AWS/aws-cpp-sdk-s3/ -laws-cpp-sdk-s3

INCLUDEPATH += $$PWD/AWS/aws-cpp-sdk-core/include
INCLUDEPATH += $$PWD/AWS/aws-cpp-sdk-s3/include
