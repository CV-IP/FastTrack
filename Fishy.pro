#-------------------------------------------------
#
# Project created by QtCreator 2018-01-26T21:49:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Fishy
TEMPLATE = app

DESTDIR=build
OBJECTS_DIR=build
MOC_DIR=build
UI_DIR=build
RCC_DIR=build


QMAKE_LFLAGS_RELEASE += -O3

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        functions.cpp \
        setupwindow.cpp \
        Hungarian.cpp

QMAKE_CXXFLAGS += -std=c++11 -O3 -fopenmp -g

INCLUDEPATH += /usr/local/include/opencv4/
LIBS += -L /usr/local/lib64/  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_video -lopencv_photo -fopenmp
HEADERS += \
        mainwindow.h\
        functions.h \
        setupwindow.h \
        Hungarian.h


FORMS += mainwindow.ui \
        setupwindow.ui 

RESOURCES += resources.qrc

