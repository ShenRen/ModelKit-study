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

INCLUDEPATH += $$PWD/../../include \
                $$PWD/../../3rdparty

#DEPENDPATH += $$INCLUDEPATH \
#              E:/Library_useVS2013/CGAL-4.7/auxiliary/gmp/lib \
#              E:/Library_useVS2013/CGAL-4.7/build_as_binaries/bin \
#              E:/Library_useVS2013/CGAL-4.7/build_as_binaries/bin/Debug \
#              E:/Library_useVS2013/boost_1_57_0_32/lib32-msvc-12.0

LIBS   += -lCGAL-vc120-mt-gd-4.7 -LE:/Library_useVS2013/CGAL-4.7/build_as_binaries/lib \
          -llibboost_thread-vc120-mt-gd-1_57 -LE:/Library_useVS2013/boost_1_57_0_32/lib32-msvc-12.0 \
          -llibgmp-10 -LE:/Library_useVS2013/CGAL-4.7/auxiliary/gmp/lib \
          -llibmpfr-4 -LE:/Library_useVS2013/CGAL-4.7/auxiliary/gmp/lib

HEADERS += \
    infillplotwidget.h \
    draw.h \
    window.h
