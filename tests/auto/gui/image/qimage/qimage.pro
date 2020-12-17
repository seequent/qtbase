CONFIG += testcase
TARGET = tst_qimage
SOURCES  += tst_qimage.cpp

QT += core-private gui-private testlib
qtConfig(c++11): CONFIG += c++11

android:!android-embedded: RESOURCES += qimage.qrc

win32: LIBS += -luser32 -lgdi32
darwin: LIBS += -framework CoreGraphics

TESTDATA += images/*
