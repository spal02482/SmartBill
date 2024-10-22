#-------------------------------------------------
#
# Project created by QtCreator 2018-11-14T21:08:10
#
#-------------------------------------------------

QT       += core gui sql printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartBill
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

# The Date Format used in the entire Application is 'ddd MMM dd yyyy' i.e. Fri Jan 04 2019

CONFIG += c++14

SOURCES += \
        main.cpp \
    login.cpp \
    invoice.cpp \
    product.cpp \
    smartbill.cpp \
    company.cpp

HEADERS += \
    login.h \
    invoice.h \
    product.h \
    smartbill.h \
    company.h

FORMS += \
    login.ui \
    invoice.ui \
    product.ui \
    smartbill.ui \
    company.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    smartbill.qrc
