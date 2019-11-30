QT += gui widgets core

CONFIG += c++17 console
CONFIG += app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        textedit.cpp

HEADERS += \
    mainwindow.h \
    textedit.h
