#-------------------------------------------------
#
# Project created by QtCreator 2014-06-21T18:29:48
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = http
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    httpserver.cpp \
    httpresponse.cpp \
    httprequest.cpp

HEADERS += \
    httpserver.h \
    httpresponse.h \
    httprequest.h
