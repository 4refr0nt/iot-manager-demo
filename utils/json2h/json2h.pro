QT += core
QT -= gui

CONFIG += c++11

TARGET = json2h
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

DEFINES += QT_DEPRECATED_WARNINGS

DISTFILES += README
