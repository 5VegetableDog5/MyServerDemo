AQT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    callingclientsuiitem.cpp \
    clientsocketitem.cpp \
    global_volume.cpp \
    main.cpp \
    mainwindow.cpp \
    onlineclientuiitem.cpp \
    server.cpp

HEADERS += \
    callingclientsuiitem.h \
    clientsocketitem.h \
    config.h \
    mainwindow.h \
    onlineclientuiitem.h \
    server.h

FORMS += \
    callingclientsuiitem.ui \
    mainwindow.ui

QT += network

INCLUDEPATH += ../externalLib/libsndfile/include
LIBS += ../externalLib/libsndfile/lib/libsndfile-1.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
