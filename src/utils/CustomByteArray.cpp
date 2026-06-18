
#include "CustomByteArray.h"
#include <cmath>
#include <stdint.h>

CustomByteArray::CustomByteArray()
{

}

CustomByteArray::CustomByteArray(const QByteArray &data) : QByteArray(data)
{

}

void CustomByteArray::appendUint64(quint64 number)
{
    appendData(number);
}

void CustomByteArray::appendInt32(qint32 number)
{
    appendData(number);
}

void CustomByteArray::appendUint32(quint32 number)
{
    appendData(number);
}

void CustomByteArray::appendInt16(qint16 number)
{
    appendData(number);
}

void CustomByteArray::appendUint16(quint16 number)
{
    appendData(number);
}

void CustomByteArray::appendInt8(qint8 number)
{
    appendData(number);
}

void CustomByteArray::appendUint8(quint8 number)
{
    appendData(number);
}

void CustomByteArray::appendFloat(float number)
{
    appendData(number);
}

qint32 CustomByteArray::popFrontInt32()
{
    if (size() < 4) {
        return 0;
    }

    qint32 res = ((quint8)at(3)) << 24 |
        ((quint8)at(2)) << 16 |
        ((quint8)at(1)) << 8 |
        ((quint8)at(0));

    remove(0, 4);
    return res;
}

quint32 CustomByteArray::popFrontUint32()
{
    if (size() < 4) {
        return 0;
    }

    quint32 res =	((quint8) at(3)) << 24 |
                    ((quint8) at(2)) << 16 |
                    ((quint8) at(1)) << 8 |
                    ((quint8) at(0));

    remove(0, 4);
    return res;
}

qint16 CustomByteArray::popFrontInt16()
{
    if (size() < 2) {
        return 0;
    }

    qint16 res =	((quint8) at(1)) << 8 |
                    ((quint8) at(0));

    remove(0, 2);
    return res;
}

quint16 CustomByteArray::popFrontUint16()
{
    if (size() < 2) {
        return 0;
    }

    quint16 res =	((quint8) at(1)) << 8 |
                    ((quint8) at(0));

    remove(0, 2);
    return res;
}

qint8 CustomByteArray::popFrontInt8()
{
    if (size() < 1) {
        return 0;
    }

    qint8 res = (qint8)at(0);

    remove(0, 1);
    return res;
}

quint8 CustomByteArray::popFrontUint8()
{
    if (size() < 1) {
        return 0;
    }

    quint8 res = (quint8)at(0);

    remove(0, 1);
    return res;
}

float CustomByteArray::popFrontFloat()
{
    uint32_t res = popFrontUint32();

    int e = (res >> 23) & 0xFF;
    int fr = res & 0x7FFFFF;
    bool negative = res & (1 << 31);

    float f = 0.0;
    if (e != 0 || fr != 0) {
        f = (float)fr / (8388608.0 * 2.0) + 0.5;
        e -= 126;
    }

    if (negative) {
        f = -f;
    }

    return ldexpf(f, e);
}
