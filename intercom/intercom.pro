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
    history.cpp \
    main.cpp \
    mainwindow.cpp \
    odbc.cpp \
    onlineclientuiitem.cpp \
    server.cpp

HEADERS += \
    callingclientsuiitem.h \
    clientsocketitem.h \
    config.h \
    history.h \
    mainwindow.h \
    odbc.h \
    onlineclientuiitem.h \
    server.h

FORMS += \
    callingclientsuiitem.ui \
    history.ui \
    mainwindow.ui

QT += network \
    multimedia \
    sql



INCLUDEPATH += ../externalLib/libsndfile/include \
     ../externalLib/SFML/include


LIBS += ../externalLib/libsndfile/lib/libsndfile-1.lib \
    ../externalLib/SFML/lib/libsfml-audio.a \
    ../externalLib/SFML/lib/libsfml-system.a \



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
