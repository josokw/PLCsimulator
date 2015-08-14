#-------------------------------------------------
#
# Project created by QtCreator 2015-07-29T16:14:47
#
#-------------------------------------------------

QT -= core gui

TEMPLATE = lib
CONFIG += c++11
CONFIG += staticlib

INCLUDEPATH += ../../VirtualPLC
DEPENDPATH += ../../VirtualPLC

CONFIG(debug, debug|release) {
    TARGET = CompilerPLCdebug
}
CONFIG(release, debug|release) {
    TARGET = CompilerPLC
    DEFINES += NDEBUG
}

SOURCES += \
   Compiler.cpp

HEADERS += \
   Compiler.h \
   Parser.h \
   LibInfoCPLC.h

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
