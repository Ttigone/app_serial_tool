#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "src/core/serial_manager.h"

#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif

/*
 * 
 *1:16:49: 正在启动 "D:\Qt6\6.9.3\mingw_64\bin\androiddeployqt.exe" --input F:/1/android-app_serial_tool-deployment-settings.json --output F:/1/android-build-app_serial_tool --android-platform android-36.1 --jdk E:/path_ware/OpenJDK17.0.19_10/jdk-17.0.19+10 --gradle

Generating Android Package
  Input file: F:/1/android-app_serial_tool-deployment-settings.json
  Output directory: F:/1/android-build-app_serial_tool/
  Application binary: app_serial_tool
  Android build platform: android-36.1
  Install to device: No
Warning: QML import could not be resolved in any of the import paths: QtQuick.Controls.Windows
Warning: QML import could not be resolved in any of the import paths: QtQuick.Controls.macOS
Warning: QML import could not be resolved in any of the import paths: QtQuick.Controls.iOS
Warning: QML import could not be resolved in any of the import paths: FluentUI
Warning: QML import could not be resolved in any of the import paths: QmlCustomPlot
Warning: QML import could not be resolved in any of the import paths: QmlCustomEditPlot
Warning: QML import could not be resolved in any of the import paths: QtGraphicalEffects
Starting a Gradle Daemon, 1 incompatible and 1 stopped Daemons could not be reused, use --status for details

FAILURE: Build failed with an exception.

* What went wrong:
A problem occurred configuring root project 'app_serial_tool'.
> Could not resolve all artifacts for configuration ':classpath'.
   > Could not resolve com.android.tools.build:gradle:8.5.0.
     Required by:
         project :
      > Could not resolve com.android.tools.build:gradle:8.5.0.
         > Could not get resource 'https://maven.aliyun.com/repository/google/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
            > Could not GET 'https://maven.aliyun.com/repository/google/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
               > Got socket exception during request. It might be caused by SSL misconfiguration
                  > Permission denied: getsockopt
      > Could not resolve com.android.tools.build:gradle:8.5.0.
         > Could not get resource 'https://maven.aliyun.com/repository/public/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
            > Could not GET 'https://maven.aliyun.com/repository/public/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
               > Got socket exception during request. It might be caused by SSL misconfiguration
                  > Permission denied: getsockopt
      > Could not resolve com.android.tools.build:gradle:8.5.0.
         > Could not get resource 'https://maven.aliyun.com/repository/gradle-plugin/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
            > Could not GET 'https://maven.aliyun.com/repository/gradle-plugin/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
               > Got socket exception during request. It might be caused by SSL misconfiguration
                  > Permission denied: getsockopt
      > Could not resolve com.android.tools.build:gradle:8.5.0.
         > Could not get resource 'https://dl.google.com/dl/android/maven2/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
            > Could not GET 'https://dl.google.com/dl/android/maven2/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
               > Got socket exception during request. It might be caused by SSL misconfiguration
                  > Permission denied: getsockopt
      > Could not resolve com.android.tools.build:gradle:8.5.0.
         > Could not get resource 'https://repo.maven.apache.org/maven2/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
            > Could not GET 'https://repo.maven.apache.org/maven2/com/android/tools/build/gradle/8.5.0/gradle-8.5.0.pom'.
               > Got socket exception during request. It might be caused by SSL misconfiguration
                  > Permission denied: getsockopt

* Try:
> Run with --stacktrace option to get the stack trace.
> Run with --info or --debug option to get more log output.
> Run with --scan to get full insights.
> Get more help at https://help.gradle.org.

BUILD FAILED in 34s
Building the android package failed!
  -- For more information, run this command with --verbose.
The maximum path length that can be processed by Gradle on Windows is 260 characters.
Consider moving your project to reduce its path length.
The following files have too long paths:
F:/1/android-build-app_serial_tool/src/org/sinemo/utils/QShareUtils.java
F:/1/android-build-app_serial_tool/src/org/sinemo/utils/ShareIntent.java
F:/1/android-build-app_serial_tool/src/s/t/UsbSerialHelper.java.
11:17:50: The command "D:\Qt6\6.9.3\mingw_64\bin\androiddeployqt.exe --input F:/1/android-app_serial_tool-deployment-settings.json --output F:/1/android-build-app_serial_tool --android-platform android-36.1 --jdk E:/path_ware/OpenJDK17.0.19_10/jdk-17.0.19+10 --gradle --gdbserver" terminated with exit code 14.
11:17:50: Error while building/deploying project app_serial_tool (kit: Qt 6.9.3 for Android arm64-v8a)
11:17:50: When executing step "构建安卓 APK"
11:17:50: Elapsed time: 01:02.

*/

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

#ifdef Q_OS_ANDROID
    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/bindings/QtActivity",
        "getInstance",
        "()Lorg/qtproject/qt/android/bindings/QtActivity;"
    );
    if (activity.isValid()) {
        QJniObject::callStaticMethod<void>(
            "s/t/UsbSerialHelper",
            "initContext",
            "(Landroid/content/Context;)V",
            activity.object<jobject>()
        );
        app.setProperty("androidContext", QVariant::fromValue(activity.object<jobject>()));
    }
#endif

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
