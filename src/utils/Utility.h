#pragma once
#include <QObject>
#include <QMetaEnum>
#include <QString>
#include <QByteArray>


#include "UtilsDefine.h"
#include "Comm/CommDefine.h"

class QQmlEngine;
class QJSEngine;

class Utility : public QObject
{
    Q_OBJECT
public:
    Q_DISABLE_COPY(Utility)

public:
    static QObject* qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine);
    static Utility* singleton();

    explicit Utility(QObject* parent = nullptr);

    Q_INVOKABLE bool requestFilePermission();
    Q_INVOKABLE bool requestBlePermission();

    Q_INVOKABLE static QString aboutText();

    static bool FloatEqual(float a, float b, float eps = EPSILON);

    static bool FloatLess(float a, float b, float eps = EPSILON);
    static bool FloatLessEqual(float a, float b, float eps = EPSILON);

    static bool FloatGreater(float a, float b, float eps = EPSILON);
    static bool FloatGreaterEqual(float a, float b, float eps = EPSILON);

    static float RoundFloat(float x);

    static CommPacktetData decodePacket(QByteArray& data);
    static QByteArray encodePacket(CommPacktetData packet, QByteArray& data_field);

    static quint16 calcSignatureCrc16(quint64 data_type_signature, QByteArray& data);

    static quint16 calcCrc16(QByteArray& data, int start_index, int length);

    static void DelayMs(unsigned int msec);

    static QByteArray HexStr2ByteArray(QString str);

    static QString ByteArray2HexStr(QByteArray byteArr);


    template<typename QEnum>
    static QString QEnumToQString (const QEnum value) {
        return QString(QMetaEnum::fromType<QEnum>().valueToKey(value));
    }



};

