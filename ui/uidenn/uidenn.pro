QT += qml quick

CONFIG += c++11

SOURCES += main.cpp

RESOURCES += qml.qrc

release: DESTDIR = $$PWD/../../Release/uidenn/
debug:   DESTDIR = $$PWD/../../Debug/uidenn/

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

linux {
    OUTPUT_TARGET = $$DESTDIR/
}

macx {
    OUTPUT_TARGET = $$DESTDIR/$${TARGET}.app/Contents/MacOS/
}

unix {
    denn_float.CONFIG += recursive
    denn_float.target = denn_float_executable
    denn_float.commands = make -C $$PWD/../../ release SCALAR=FLOAT
    denn_float.files = $$PWD/../../Release/DENN-float
    denn_float.path = $${OUTPUT_TARGET}

    denn_double.CONFIG += recursive
    denn_double.target = denn_double_executable
    denn_double.commands = make -C $$PWD/../../ release SCALAR=DOUBLE
    denn_double.files = $$PWD/../../Release/DENN-double
    denn_double.path = $${OUTPUT_TARGET}

    denn_longdouble.CONFIG += recursive
    denn_longdouble.target = denn_longdouble_executable
    denn_longdouble.commands = make -C $$PWD/../../ release SCALAR=LONG_DOUBLE
    denn_longdouble.files = $$PWD/../../Release/DENN-long-double
    denn_longdouble.path = $${OUTPUT_TARGET}

    #add target
    QMAKE_EXTRA_TARGETS += denn_float denn_double denn_longdouble

    #make
    POST_TARGETDEPS += denn_float_executable denn_double_executable denn_longdouble_executable

}

linux {
    #copy
    QMAKE_POST_LINK += $$quote(cp -rf $$denn_float.files $$denn_float.path$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(cp -rf $$denn_double.files $$denn_double.path$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(cp -rf $$denn_longdouble.files $$denn_longdouble.path$$escape_expand(\n\t))
}

macx {

    copy_float.target = denn_float_copy
    copy_float.commands = cp $$denn_float.files $$denn_float.path

    copy_double.target = denn_double_copy
    copy_double.commands = cp $$denn_double.files $$denn_double.path

    copy_longdouble.target = denn_longdouble_copy
    copy_longdouble.commands = cp $$denn_longdouble.files $$denn_longdouble.path

    QMAKE_EXTRA_TARGETS += copy_float copy_double copy_longdouble
    POST_TARGETDEPS     += denn_float_copy denn_double_copy denn_longdouble_copy


}

win32 {

    build_denn_float = \
            MSBuild ../../DENN.sln \
           /p:Configuration=Release \
           /p:DefineConstants=USE_FLOAT \
           /p:OutDir=\"$$DESTDIR\" \
           /p:IntermediateOutputPath=\"$$PWD/../../Release/Release-float/\" \
           /p:AssemblyName=\"DENN-float\"

    build_denn_double = \
            MSBuild ../../DENN.sln \
           /p:Configuration=Release \
           /p:DefineConstants=USE_DOUBLE \
           /p:OutDir=\"$$DESTDIR\" \
           /p:IntermediateOutputPath=\"$$PWD/../../Release/Release-double/\" \
           /p:AssemblyName=\"DENN-double\"

    build_denn_long_double = \
            MSBuild ../../DENN.sln \
           /p:Configuration=Release \
           /p:DefineConstants=USE_LONG_DOUBLE \
           /p:OutDir=\"$$DESTDIR\" \
           /p:IntermediateOutputPath=\"$$PWD/../../Release/Release-long-double/\" \
           /p:AssemblyName=\"DENN-long-double\"

    system($$build_denn_float)
    system($$build_denn_double)
    system($$build_denn_long_double)
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

HEADERS += \
    QtBasicProcess.h \
    QtQmlStringUtils.h
