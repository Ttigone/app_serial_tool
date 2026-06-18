#include "FluPlugin.h"
#include <QtQml/QQmlExtensionPlugin>
#include <QGuiApplication>
#include <qdebug.h>
#include "FluDef.h"
#include "FluApp.h"
#include "FluColors.h"
#include "FluTheme.h"
#include "FluTools.h"
#include "FluTextStyle.h"
int major = 1;
int minor = 0;
void FluentUIPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<FluColorSet>(uri,major,minor,"FluColorSet");


    qmlRegisterUncreatableType<FluThemeType>(uri, major, minor, "FluThemeType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluPageType>(uri, major, minor, "FluPageType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluWindowType>(uri, major, minor, "FluWindowType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluTreeViewType>(uri, major, minor, "FluTreeViewType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluStatusViewType>(uri, major, minor, "FluStatusViewType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluContentDialogType>(uri, major, minor, "FluContentDialogType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluTimePickerType>(uri, major, minor, "FluTimePickerType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluCalendarViewType>(uri, major, minor, "FluCalendarViewType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluTabViewType>(uri, major, minor, "FluTabViewType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluNavigationViewType>(uri, major, minor, "FluNavigationViewType", "Access to enums & flags only");
    qmlRegisterUncreatableType<FluentIcons>(uri, major, minor, "FluentIcons", "Access to enums & flags only");

//    qmlRegisterUncreatableMetaObject(Fluent_DarkMode::staticMetaObject,  uri,major,minor,"FluDarkMode", "Access to enums & flags only");
//    qmlRegisterUncreatableMetaObject(Fluent_Awesome::staticMetaObject,  uri,major,minor,"FluentIcons", "Access to enums & flags only");
}

void FluentUIPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
#ifdef Q_OS_WIN
//    QFont font;
//    font.setFamily("Microsoft YaHei");
//    QGuiApplication::setFont(font);
#endif
    FluApp* app = FluApp::getInstance();
    engine->rootContext()->setContextProperty("FluApp",app);
    FluColors* colors = FluColors::getInstance();
    engine->rootContext()->setContextProperty("FluColors",colors);
    FluTheme* theme = FluTheme::getInstance();
    engine->rootContext()->setContextProperty("FluTheme",theme);
    FluTools* tools = FluTools::getInstance();
    engine->rootContext()->setContextProperty("FluTools",tools);
    FluTextStyle* textStyle = FluTextStyle::getInstance();
    engine->rootContext()->setContextProperty("FluTextStyle",textStyle);
}
