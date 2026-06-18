#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

#include "Public/AppInterface.h"
//#include "Utils/Trans.h"


class MobileMain : public QObject
{
    Q_OBJECT
public:
    explicit MobileMain(QObject *parent = nullptr);

    bool startQmlUi();

    bool eventFilter(QObject *object, QEvent *e);

    void setVisible(bool visible);

private:
    QQmlApplicationEngine *m_Engine;
    //Trans* m_Trans;
};
