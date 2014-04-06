#-------------------------------------------------
#
# Project created by QtCreator 2013-12-10T11:11:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EncExp
TEMPLATE = app

DEFINES += CRYPTOPP_EXPORTS

LIBS += -lcryptopp

SOURCES += main.cpp\
        mainwindow.cpp \
    decryptor.cpp \
    encryptor.cpp \
    directories.cpp \
    progressbar.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    decryptor.h \
    encryptor.h \
    EncryptionModel.h \
    directories.h \
    progressbar.h \
    about.h

FORMS    += mainwindow.ui \
    directories.ui \
    progressbar.ui \
    about.ui

RESOURCES += \
    ICON.qrc
