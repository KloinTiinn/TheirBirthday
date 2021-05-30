#-------------------------------------------------
#
# Project created by QtCreator 2020-01-28T11:23:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TheirBurthday
TEMPLATE = app
TRANSLATIONS = main_ru.ts main_en.ts


SOURCES += main.cpp\
        editwindow.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    editwindow.h

FORMS    += mainwindow.ui \
    editwindow.ui

RESOURCES += \
    TheirBurthday.qrc
