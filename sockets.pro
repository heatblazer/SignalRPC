QT += core
QT += network
QT += gui
QT += widgets

CONFIG += c++11

TARGET = sockets
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    signalrpc.cpp \
    ptt.cpp 

HEADERS += \
    signalrpc.h \
    ptt.h 
