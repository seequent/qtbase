CONFIG += testcase
TARGET = tst_qmakelib
QT = core testlib core-private
win32: LIBS += -ladvapi32

INCLUDEPATH += ../../../../qmake/library
VPATH += ../../../../qmake/library

HEADERS += \
    tst_qmakelib.h

SOURCES += \
    tst_qmakelib.cpp \
    parsertest.cpp \
    evaltest.cpp \
    ioutils.cpp \
    registry.cpp \
    proitems.cpp \
    qmakevfs.cpp \
    qmakeparser.cpp \
    qmakeglobals.cpp \
    qmakebuiltins.cpp \
    qmakeevaluator.cpp

DEFINES += PROPARSER_DEBUG PROEVALUATOR_FULL PROEVALUATOR_SETENV QT_USE_QSTRINGBUILDER
