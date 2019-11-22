QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ../../Common

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../Common/analyze_atpmessage.cpp \
    ../../Common/analyze_resmessage.cpp \
    ../../Common/basestructer.cpp \
    ../../Common/Command.cpp \
    ../../Common/designdata.cpp \
    baliselocation.cpp \
    balisestation.cpp \
    inner_tcp_client.cpp \
    main.cpp \
    compareresdata_thread.cpp \
    self_define.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../Common/Command.h \
    ../../Common/TcpAbstract.h \
    ../../Common/analyze_atpmessage.h \
    ../../Common/analyze_resmessage.h \
    ../../Common/basestructer.h \
    ../../Common/designdata.h \
    baliselocation.h \
    balisestation.h \
    compareresdata_thread.h \
    inner_tcp_client.h \
    self_define.h
