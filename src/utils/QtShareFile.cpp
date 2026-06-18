#include "QtShareFile.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QApplication>
#include "Data/ParamManager.h"
#include <QDateTime>
#include <QLocale>

#ifdef Android_Platform
#include <QAndroidJniObject>
#include <QtAndroid>

#endif

QObject* QtShareFile::qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine){
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    static QtShareFile* _singleton=nullptr;
    if(!_singleton)
        _singleton = new QtShareFile;
    return _singleton;
}

QtShareFile* QtShareFile::singleton(){
    return qobject_cast<QtShareFile*>(qmlSingleton(nullptr,nullptr));
}


QtShareFile::QtShareFile(QObject* parent)
    : QObject(parent)
{

}

QtShareFile::~QtShareFile()
{

}

void QtShareFile::shareFile(QString file_name)
{
#ifdef Android_Platform
    qDebug() << "share-----loading-------------:" ;

    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject file_path = QAndroidJniObject::fromString(file_name);

    QAndroidJniObject::callStaticMethod<void>(
                "org/sinemo/utils/ShareIntent",
                "shareFile",
                "(Lorg/qtproject/qt5/android/bindings/QtActivity;Ljava/lang/String;)V",
                activity.object<jobject>(),
                file_path.object<jstring>());


//    int requestId = -3;
//    QAndroidJniObject jsPath = QAndroidJniObject::fromString(file_name);
//    QAndroidJniObject jsTitle = QAndroidJniObject::fromString("Send File");
//    QAndroidJniObject jsMimeType = QAndroidJniObject::fromString("");
//    jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>("org/sinemo/utils/QShareUtils",
//                                              "sendFile",
//                                              "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)Z",
//                                              jsPath.object<jstring>(), jsTitle.object<jstring>(), jsMimeType.object<jstring>(), requestId);

//    jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>("org/sinemo/utils/QShareUtils",
//                                              "sendFile",
//                                               "(Lorg/qtproject/qt5/android/bindings/QtActivity;)V",
//                                              jsPath.object<jstring>(), jsTitle.object<jstring>(), jsMimeType.object<jstring>(), requestId);

//    if(!ok) {
//        qWarning() << "Unable to resolve activity from Java";
//        //emit shareNoAppAvailable(requestId);
//    }
#endif
}
