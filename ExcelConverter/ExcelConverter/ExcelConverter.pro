#-------------------------------------------------
#
# Project created by QtCreator 2019-09-26T21:48:13
#
#-------------------------------------------------

QT += core gui
QT += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ExcelConverter
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    ../../Common/basestructer.cpp \
    ../../Common/designdata.cpp \
        excel2qstr.cpp \
        main.cpp \
        mainwindow.cpp \
        rulecommon.cpp \
        rules_chezhan.cpp \
        rules_duanlian.cpp \
        rules_guofenxiang.cpp \
        rules_jinlu.cpp \
        rules_podu.cpp \
        rules_sudu.cpp \
        rules_xianlushuju.cpp \
        rules_yingdaqiweizhi.cpp \
        rules_zhantai.cpp


HEADERS += \
    ../../Common/basestructer.h \
    ../../Common/designdata.h \
        excel2qstr.h \
        mainwindow.h \
        rulecommon.h \
        rules_chezhan.h \
        rules_duanlian.h \
        rules_guofenxiang.h \
        rules_jinlu.h \
        rules_podu.h \
        rules_sudu.h \
        rules_xianlushuju.h \
        rules_yingdaqiweizhi.h \
        rules_zhantai.h


FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
