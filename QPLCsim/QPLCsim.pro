# QPLCsim.pro 2009-12-05

QT += widgets
TARGET = PLCsim
TEMPLATE = app
CONFIG += c++14

INCLUDEPATH +=  ../VirtualPLC
DEPENDPATH += ../VirtualPLC
INCLUDEPATH += ../CompilerPLC
DEPENDPATH += ../CompilerPLC

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
        LIBS += -L$$OUT_PWD/../VirtualPLC -lVirtualPLCdebug
        #PRE_TARGETDEPS += ../debug/VirtualPLC/libVirtualPLCdebug.a
        LIBS += -L$$OUT_PWD/../CompilerPLC -lCompilerPLCdebug
        #PRE_TARGETDEPS += ../../debug/CompilerPLC/libCompilerPLCdebug.a
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$OUT_PWD/../VirtualPLC -lVirtualPLC
        #PRE_TARGETDEPS += ../../release/VirtualPLC/libVirtualPLC.a
        LIBS += -L$$OUT_PWD/../CompilerPLC -lCompilerPLC
        #PRE_TARGETDEPS += ../../release/CompilerPLC/libCompilerPLC.a
    }
}
