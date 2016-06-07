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
    ../3rdparty/xd/pgn_print.h

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
               $$PWD/../3rdparty \
#               E:/Library_useVS2013/CGAL-4.7/include\
#               E:/Library_useVS2013/CGAL-4.7/auxiliary/gmp/include \
#               E:/Library_useVS2013/CGAL-4.7/build_as_binaries/include \
#               E:/Library_useVS2013/boost_1_57_0_32 \

#              D:/boost_1_57_0 D:/CGAL-4.5.1/include D:/CGAL-4.5.1/auxiliary/gmp/include

#LIBS   += -lCGAL-vc120-mt-gd-4.7 -LE:/Library_useVS2013/CGAL-4.7/build_as_binaries/lib \
         # -llibboost_thread-vc120-mt-gd-1_57 -LE:/Library_useVS2013/boost_1_57_0_32/lib32-msvc-12.0

#LIBS   += -llibgmp-10.lib -LD:/CGAL-4.5.1/auxiliary/gmp/lib \
#          -llibmpfr-4.lib -LD:/CGAL-4.5.1/auxiliary/gmp/lib

#INCLUDEPATH += $$PWD/../../include
#DEPENDPATH += $$PWD/../../include

#DEPENDPATH += $$INCLUDEPATH \
#              E:/Library_useVS2013/CGAL-4.7/auxiliary/gmp/lib \
#              E:/Library_useVS2013/CGAL-4.7/build_as_binaries/bin \
#              E:/Library_useVS2013/CGAL-4.7/build_as_binaries/bin/Debug \
#              E:/Library_useVS2013/boost_1_57_0_32/lib32-msvc-12.0
