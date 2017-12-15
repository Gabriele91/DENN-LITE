#-------------------------------------------------
#
# Project created by QtCreator 2017-12-11T12:38:27
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NRamNodes
TEMPLATE = app


DEFINES += QT_DEPRECATED_WARNINGS

#INCLUDE LIBS
INCLUDEPATH += $$PWD/../dips/include
DEFINES += NODE_EDITOR_SHARED

#Mac Dips
macx {
    #link lib
    LIBS += $$PWD/../dips/macOS/libnodes.dylib
    #add lib in bundle
    RESFILES.files =  $$PWD/../dips/macOS/libnodes.dylib
    RESFILES.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += RESFILES
    QMAKE_RPATHDIR += @executable_path/../Frameworks

}

SOURCES += \
        main.cpp \
        NRamNodes.cpp \
    NRamGate.cpp \
    NRamRegister.cpp \
    NRamMemory.cpp

HEADERS += \
        NRamNodes.h \
    NRamGate.h \
    NRamRegister.h \
    NRamMemory.h

FORMS += \
        NRamNodes.ui

RESOURCES += \
    qdarkstyle/style.qrc
