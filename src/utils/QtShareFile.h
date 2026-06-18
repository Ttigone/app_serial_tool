#pragma once
#include <QObject>
#include <QString>
#include <QMap>
#include "Data/TestCasePoint.h"

class QQmlEngine;
class QJSEngine;

class QtShareFile : public QObject
{
    Q_OBJECT
public:
    Q_DISABLE_COPY(QtShareFile)

public:
    static QObject* qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine);
    static QtShareFile* singleton();

    explicit QtShareFile(QObject* parent = nullptr);
    ~QtShareFile();

    Q_INVOKABLE void shareFile(QString file_name);

};

