#-------------------------------------------------
#
# Project created by QtCreator 2015-02-28T12:03:12
#
#-------------------------------------------------

unix:QMAKE_CXXFLAGS += "-fno-sized-deallocation" # error _ZdlPvm workaround

QT       += core gui sql xml printsupport network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kcrm
TEMPLATE = app


SOURCES += main.cpp\
    clientcalldialog.cpp \
    clientscolorsqlquerymodel.cpp \
    directiondialog.cpp \
    directionsdialog.cpp \
        mainwindow.cpp \
    common_inc.cpp \
    proverkadialog.cpp \
    trestdialog.cpp \
    trestsdialog.cpp \
    xmlsettings.cpp \
    requestdialog.cpp \
    selectconnectiondialog.cpp \
    clientsdialog.cpp \
    clientdialog.cpp \
    colorsqlquerymodel.cpp \
    aes.cpp \
    optionsdialog.cpp \
    optiondialog.cpp \
    officesdialog.cpp \
    officedialog.cpp \
    eventsdialog.cpp \
    calendardialog.cpp \
    agreementdialog.cpp \
    sendsmsdialog.cpp \
    devicedialog.cpp \
    clientdevicedialog.cpp \
    agreementscolorsqlquerymodel.cpp \
    reminddialog.cpp \
    clearamountsdialog.cpp \
    userdialog.cpp \
    usersdialog.cpp

HEADERS  += mainwindow.h \
    clientcalldialog.h \
    clientscolorsqlquerymodel.h \
    common_inc.h \
    directiondialog.h \
    directionsdialog.h \
    proverkadialog.h \
    trestdialog.h \
    trestsdialog.h \
    xmlsettings.h \
    requestdialog.h \
    selectconnectiondialog.h \
    clientsdialog.h \
    clientdialog.h \
    colorsqlquerymodel.h \
    aes.h \
    optionsdialog.h \
    optiondialog.h \
    officesdialog.h \
    officedialog.h \
    eventsdialog.h \
    calendardialog.h \
    agreementdialog.h \
    sendsmsdialog.h \
    devicedialog.h \
    clientdevicedialog.h \
    agreementscolorsqlquerymodel.h \
    reminddialog.h \
    printdialog.h \
    clearamountsdialog.h \
    userdialog.h \
    usersdialog.h

FORMS    += mainwindow.ui \
    clientcalldialog.ui \
    directiondialog.ui \
    directionsdialog.ui \
    proverkadialog.ui \
    requestdialog.ui \
    selectconnectiondialog.ui \
    clientsdialog.ui \
    clientdialog.ui \
    optionsdialog.ui \
    optiondialog.ui \
    officesdialog.ui \
    officedialog.ui \
    eventsdialog.ui \
    calendardialog.ui \
    agreementdialog.ui \
    sendsmsdialog.ui \
    devicedialog.ui \
    clientdevicedialog.ui \
    reminddialog.ui \
    clearamountsdialog.ui \
    trestdialog.ui \
    trestsdialog.ui \
    userdialog.ui \
    usersdialog.ui

RESOURCES += \
    kcrm.qrc


greaterThan(QT_MINOR_VERSION, 5) {
        QT += webenginewidgets
        SOURCES += printdialog.cpp
        FORMS += printdialog.ui
} else {
        QT += webkitwidgets
        SOURCES += printdialog_webkit.cpp
        FORMS += printdialog_webkit.ui
}


win32:RC_FILE = kcrm.rc
win64:RC_FILE = kcrm.rc
macx:ICON = images/kcrm.icns
