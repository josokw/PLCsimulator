# QPLCsim.pro 2009-12-05

QT += widgets
TARGET = PLCsim
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += ../../VirtualPLC
DEPENDPATH += ../../VirtualPLC

SOURCES += main.cpp \
    mainwindow.cpp \
    QLineNumberBar.cpp \
    QScriptEditor.cpp \
    QPlcDialog.cpp \
    QPlcReadY.cpp \
    QLed.cpp \
    QSyntaxHL.cpp
HEADERS += mainwindow.h \
    QLineNumberBar.h \
    QScriptEditor.h \
    QPlcDialog.h \
    QPlcReadY.h \
    QLed.h \
    AppInfo.h \
    QSyntaxHL.h
FORMS += mainwindow.ui
RESOURCES += plcsim.qrc

unix {
    CONFIG(debug, debug|release) {
        LIBS += -L../../debug/VirtualPLC -lVirtualPLCdebug
        PRE_TARGETDEPS += ../../debug/VirtualPLC/libVirtualPLCdebug.a
        LIBS += -L../../debug/CompilerPLC -lCompilerPLCdebug
        PRE_TARGETDEPS += ../../debug/CompilerPLC/libCompilerPLCdebug.a
    }
    CONFIG(release, debug|release) {
        LIBS += -L../../release/VirtualPLC -lVirtualPLC
        PRE_TARGETDEPS += ../../release/VirtualPLC/libVirtualPLC.a
        LIBS += -L../../release/CompilerPLC -lCompilerPLC
        PRE_TARGETDEPS += ../../release/CompilerPLC/libCompilerPLC.a
    }
}
