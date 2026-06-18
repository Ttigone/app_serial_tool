#ifndef FLUENTUIPLUGIN_H
#define FLUENTUIPLUGIN_H

#include <QObject>
#include <QQmlEngine>

class FluentUIPlugin : public QObject
{
    Q_OBJECT

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);

};

#endif // FLUENTUIPLUGIN_H
