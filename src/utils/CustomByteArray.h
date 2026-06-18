#pragma once

#include <QByteArray>
#include <QString>
#include <QtGlobal> 
#include <QtEndian> 

class CustomByteArray : public QByteArray
{
public:
    CustomByteArray();
    CustomByteArray(const QByteArray &data);

    template <class T> void appendData(T data)
    {
        int data_size = sizeof(data);

        void* tmp_data = malloc(data_size);
        memset(tmp_data, 0, data_size);
        qToLittleEndian(data, tmp_data);

        append((const char*)tmp_data, data_size);

        free(tmp_data);
    }

    void appendUint64(quint64 number);

    void appendInt32(qint32 number);
    void appendUint32(quint32 number);
    void appendInt16(qint16 number);
    void appendUint16(quint16 number);
    void appendInt8(qint8 number);
    void appendUint8(quint8 number);

    void appendFloat(float number);


    qint32 popFrontInt32();
    quint32 popFrontUint32();
    qint16 popFrontInt16();
    quint16 popFrontUint16();
    qint8 popFrontInt8();
    quint8 popFrontUint8();
    float popFrontFloat();
};

