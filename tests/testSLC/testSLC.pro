TEMPLATE = app
CONFIG -= app_bundle
CONFIG += console
QT     += core
QT     -= gui
CONFIG += c++11

DESTDIR = $$OUT_PWD/../../bin

SOURCES += \
    testslc.cpp

unix|win32: LIBS += -L$$DESTDIR -lModelKit

INCLUDEPATH += $$PWD/../../include
DEPENDPATH += $$INCLUDEPATH
