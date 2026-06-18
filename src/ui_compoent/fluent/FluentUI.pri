
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH	+= \
    $$PWD

QML2_IMPORT_PATH += \
        $$PWD

RESOURCES += $$PWD/FluentUI/FluentUI.qrc

HEADERS += \
    $$PWD/FluMacro.h \
    $$PWD/FluDef.h \
    $$PWD/FluApp.h \
    $$PWD/FluColors.h \
    $$PWD/FluColorSet.h \
    $$PWD/FluRegister.h \
    $$PWD/FluTextStyle.h \
    $$PWD/FluTheme.h \
    $$PWD/FluTools.h \
    $$PWD/FluPlugin.h



SOURCES += \
    $$PWD/FluDef.cpp \
    $$PWD/FluApp.cpp \
    $$PWD/FluColors.cpp \
    $$PWD/FluColorSet.cpp \
    $$PWD/FluRegister.cpp \
    $$PWD/FluTextStyle.cpp \
    $$PWD/FluTheme.cpp \
    $$PWD/FluTools.cpp \
    $$PWD/FluPlugin.cpp
