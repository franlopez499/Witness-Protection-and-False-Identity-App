#-------------------------------------------------
#
# Project created by QtCreator 2020-10-15T12:32:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PTestigos
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += PROJECT_PATH=\"\\\"$${_PRO_FILE_PWD_}/\\\"\"
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        controlador.cpp \
        facedetector.cpp \
        facedetectorcascade.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        controlador.h \
        facedetector.h \
        facedetectorcascade.h \
        mainwindow.h

FORMS += \
        mainwindow.ui
INCLUDEPATH += "C:\OpenCV\OpenCV4.1.1G\include"
LIBS += -L"C:\OpenCV\OpenCV4.1.1G\lib"\
 -llibopencv_world411

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
