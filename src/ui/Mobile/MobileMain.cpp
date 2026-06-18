#include "MobileMain.h"

#include <QQuickWindow>
#include <QQmlContext>

#include <QQuickStyle>
#include <QApplication>


#include "Comm/BleUart.h"
#include "Comm/CommManager.h"
#include "Data/ParamManager.h"
#include "Data/RealTimeDataManager.h"
#include "Data/TestDataManager.h"

#include "CustomPlot/QmlCustomPlot.h"
#include "CustomPlot/QmlCustomEditPlot.h"

#include "Utils/Utility.h"
#include "Utils/Config.h"
#include "widget/FluPlugin.h"
#include "Utils/QtShareFile.h"

MobileMain::MobileMain(QObject *parent)
    : QObject(parent)
{
    m_Engine = new QQmlApplicationEngine(this);
    //m_Trans = new Trans(this);

    QString root_path = Config::singleton()->getRootPath();
    m_Engine->rootContext()->setContextProperty("rootDirPath", root_path);

    m_Engine->rootContext()->setContextProperty("AppInterface", AppInterface::singleton());
    m_Engine->rootContext()->setContextProperty("CommManager", CommManager::singleton());
    m_Engine->rootContext()->setContextProperty("SerialDevice", CommManager::singleton()->serialPort());
    m_Engine->rootContext()->setContextProperty("BleUart", CommManager::singleton()->bleUartDevice());

    m_Engine->rootContext()->setContextProperty("Command", CommManager::singleton()->command());
    m_Engine->rootContext()->setContextProperty("FirmwareUpdate", CommManager::singleton()->firmwareUpdate());

    m_Engine->rootContext()->setContextProperty("ParamManager", ParamManager::singleton());
    m_Engine->rootContext()->setContextProperty("RealtimeDataManager", RealTimeDataManager::singleton());
    m_Engine->rootContext()->setContextProperty("TestDataManager", TestDataManager::singleton());

    m_Engine->rootContext()->setContextProperty("Utility", Utility::singleton());
    m_Engine->rootContext()->setContextProperty("Config", Config::singleton());
    m_Engine->rootContext()->setContextProperty("QtShareFile", QtShareFile::singleton());


    qmlRegisterType<QmlCustomPlot>("QmlCustomPlot", 1, 0, "QmlCustomPlot");
    qmlRegisterType<QmlCustomEditPlot>("QmlCustomEditPlot", 1, 0, "QmlCustomEditPlot");

    qmlRegisterUncreatableType<CurvePlotType>("FluentUI", 1, 0, "CurvePlotType", "Access to enums & flags only");


    FluentUIPlugin flu_plugin;
    flu_plugin.registerTypes("FluentUI");

    flu_plugin.initializeEngine(m_Engine, "FluentUI");

    //qmlRegisterType<BleUart>("App.Module.BleUart", 1, 0, "BleUart");
    //qmlRegisterType<Command>("App.Module.Command", 1, 0, "Command");
    //qmlRegisterType<Parameter>("App.Module.Parameter", 1, 0, "Parameter");
}

bool MobileMain::startQmlUi()
{
    m_Engine->addImportPath(QStringLiteral("qrc:/"));
    //m_Trans->beforeUiReady(m_Engine->rootContext());

    m_Engine->load(QUrl(QLatin1String("qrc:/MobileUI/MobileMain.qml")));
    return !m_Engine->rootObjects().isEmpty();
}

bool MobileMain::eventFilter(QObject *object, QEvent *e)
{
    (void)object;

    return false;
}

void MobileMain::setVisible(bool visible)
{
    QObject *rootObject = m_Engine->rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(rootObject);
    if (window) {
        window->setVisible(visible);
    }
}
