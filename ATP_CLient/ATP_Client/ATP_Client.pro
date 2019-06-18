#-------------------------------------------------
#
# Project created by QtCreator 2018-07-25T15:04:17
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += axcontainer
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ATP_Client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../../Common

SOURCES += \
    ../../Common/analyze_rbcmessage.cpp \
    ../../Common/analyze_resmessage.cpp \
    ../../Common/basestructer.cpp \
    ../../Common/command.cpp \
    ../../Common/designdata.cpp \
    choosedesignfiles_dialog.cpp \
        main.cpp \
        mainwindow.cpp \
    analyzeresultshow_dialog.cpp \
    callout.cpp \
    chartview.cpp \
    getreport_dialog.cpp \
    Excelwork.cpp \
    readdesignfile_thread.cpp \
    showresponsermsg_dialog.cpp \
    tcpcommandclient.cpp \
    tcpdataclient.cpp \
    usermanagementui.cpp

HEADERS += \
    ../../Common/Command.h \
    ../../Common/TcpAbstract.h \
    ../../Common/analyze_rbcmessage.h \
    ../../Common/analyze_resmessage.h \
    ../../Common/basestructer.h \
    ../../Common/designdata.h \
    choosedesignfiles_dialog.h \
    encrypt.h \
        mainwindow.h \
    Config_arg.h \
    readdesignfile_thread.h \
    self_define.h \
    analyzeresultshow_dialog.h \
    callout.h \
    chartview.h \
    getreport_dialog.h \
    Excelwork.h \
    showresponsermsg_dialog.h \
    tcpcommandclient.h \
    tcpdataclient.h \
    usermanagementui.h

FORMS += \
    choosedesignfiles.ui \
        mainwindow.ui \
    rbcshow_dialog.ui \
    getreport_dialog.ui \
    showresponsermsg_dialog.ui \
    usermanagementui.ui
