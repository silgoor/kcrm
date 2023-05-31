FORMS           = settingsdialog.ui \
    connectiondialog.ui
HEADERS         = ../common_inc.h settingsdialog.h ../xmlsettings.h \
    connectiondialog.h \
    ../aes.h
SOURCES         = main.cpp ../common_inc.cpp settingsdialog.cpp ../xmlsettings.cpp \
    connectiondialog.cpp \
    ../aes.cpp
QT              += sql xml widgets network
win32:RC_FILE = kcrm_settings.rc
macx:ICON = images/kcrm_settings.icns

RESOURCES += \
    ../kcrm.qrc
