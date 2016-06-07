TEMPLATE = subdirs

CONFIG += ordered \
          c++11

SUBDIRS += src \
           tests

QT +=gui widgets
