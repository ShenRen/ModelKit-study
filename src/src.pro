#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T13:17:43
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += static
TARGET = ModelKit
QT       += core gui
QT += widgets
CONFIG += c++11
#LIBS   += -lQt5Gui.a -LD:/Qt/Qt5.5.1/5.5/mingw492_32/lib

DESTDIR = $$OUT_PWD/../bin

HEADERS += \
    ../include/boundary.h \
    ../include/layer.h \
    ../include/math.hpp \
    ../include/point.h \
    ../include/polygon.h \
    ../include/slcmodel.h \
    ../3rdparty/clipper/clipper.hpp \
    ../3rdparty/cura/infill.h \
    ../3rdparty/cura/polygonUtils.h \
    ../3rdparty/xd/generate_line.h \

SOURCES += \
    point.cpp \
    polygon.cpp \
    layer.cpp \
    slcmodel.cpp \
    boundary.cpp \
    ../3rdparty/clipper/clipper.cpp \
    ../3rdparty/cura/infill.cpp \
    ../3rdparty/xd/generate_line.cpp

INCLUDEPATH += $$PWD/../include \
               $$PWD/../3rdparty

#INCLUDEPATH += $$PWD/../../include
#DEPENDPATH += $$PWD/../../include
