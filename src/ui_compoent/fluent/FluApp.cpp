#include "FluApp.h"

#include <QQmlEngine>
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickItem>
#include <QTimer>
#include <QUuid>
#include <QFontDatabase>
#include <QClipboard>
#include "FluDef.h"

FluApp* FluApp::m_instance = nullptr;

FluApp *FluApp::getInstance()
{
    if(FluApp::m_instance == nullptr){
        FluApp::m_instance = new FluApp;
    }
    return FluApp::m_instance;
}

FluApp::FluApp(QObject *parent)
    : QObject{parent}
{
}

FluApp::~FluApp(){
}

void FluApp::init(QQuickWindow *window){
    this->appWindow = window;
}

void FluApp::run(){
    navigate(initialRoute());
}

void FluApp::navigate(const QString& route,const QJsonObject& argument,FluRegister* fluRegister){
    if(!routes().contains(route)){
        qCritical()<<"No route found "<<route;
        return;
    }
    QQmlEngine *engine = qmlEngine(appWindow);
    QQmlComponent component(engine, routes().value(route).toString());
    if (component.isError()) {
        qCritical() << component.errors();
        return;
    }
    QVariantMap properties;
    properties.insert("route",route);
    if(fluRegister){
        properties.insert("pageRegister",QVariant::fromValue(fluRegister));
    }
    properties.insert("argument",argument);
    QQuickWindow *view=nullptr;
    for (auto& pair : wnds) {
        QString r =  pair->property("route").toString();
        if(r == route){
            view = pair;
            break;
        }
    }
    if(view){
        //如果窗口存在，则判断启动模式
        int launchMode = view->property("launchMode").toInt();
        if(launchMode == 1){
            view->setProperty("argument",argument);
            view->show();
            view->raise();
            view->requestActivate();
            return;
        }else if(launchMode == 2){
            view->close();
        }
    }
    //view = qobject_cast<QQuickWindow*>(component.createWithInitialProperties(properties));
    wnds.insert(view->winId(),view);
    if(fluRegister){
        fluRegister->to(view);
    }
    view->setColor(QColor(Qt::transparent));
}


void FluApp::closeApp(){
    qApp->exit(0);
}

void FluApp::deleteWindow(QQuickWindow* window){
    if(window){
        wnds.remove(window->winId());
        window->deleteLater();
        window = nullptr;
    }
}
