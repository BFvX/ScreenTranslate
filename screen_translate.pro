QT       += core gui widgets network

CONFIG   += c++11

TARGET = screen_translate
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/screensnippet.cpp \
    src/geminiapihandler.cpp \
    src/settingsdialog.cpp \
    src/framelessdialog.cpp \
    src/titlebar.cpp

HEADERS  += \
    src/mainwindow.h \
    src/screensnippet.h \
    src/geminiapihandler.h \
    src/settingsdialog.h \
    src/framelessdialog.h \
    src/titlebar.h

RESOURCES += res/resources.qrc
