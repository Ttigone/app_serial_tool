#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "src/core/serial_manager.h"

#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

#ifdef Q_OS_ANDROID
    // initialize Java USB helper with Android context
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/bindings/QtActivity",
        "getInstance",
        "()Lorg/qtproject/qt/android/bindings/QtActivity;"
    );
    if (activity.isValid()) {
        QJniObject::callStaticMethod<void>(
            "com/serialtool/UsbSerialHelper",
            "initContext",
            "(Landroid/content/Context;)V",
            activity.object<jobject>()
        );
        app.setProperty("androidContext", QVariant::fromValue(activity.object<jobject>()));
    }
#endif

    // expose serial_manager to QML
    serial::serial_manager& mgr = serial::serial_manager::instance();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("serial_manager", &mgr);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("app_serial_tool", "Main");

    return QGuiApplication::exec();
}
