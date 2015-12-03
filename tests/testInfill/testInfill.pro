TEMPLATE = app
CONFIG -= app_bundle
CONFIG += console
QT     += core gui widgets
CONFIG += c++11

DESTDIR = $$OUT_PWD/../../bin

SOURCES += \
    testinfill.cpp \
    infillplotwidget.cpp \
    draw.cpp \
    window.cpp

unix|win32: LIBS += -L$$DESTDIR -lModelKit

INCLUDEPATH += $$PWD/../../include
DEPENDPATH += $$INCLUDEPATH

HEADERS += \
    infillplotwidget.h \
    draw.h \
    window.h
