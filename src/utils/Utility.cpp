#include "Utility.h"
#include <QtMath>
#include <QEventLoop>
#include <QTimer>

#include "Comm/CommDefine.h"
#include "CustomByteArray.h"

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#endif

QObject* Utility::qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine){
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    static Utility* _singleton=nullptr;
    if(!_singleton)
        _singleton=new Utility;
    return _singleton;
}

Utility* Utility::singleton(){
    return qobject_cast<Utility*>(qmlSingleton(nullptr,nullptr));
}

Utility::Utility(QObject* parent) 
    : QObject(parent)
{

}

bool Utility::requestFilePermission()
{
#ifdef Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    // https://codereview.qt-project.org/#/c/199162/
    //QString read_permission = "android.permission.READ_EXTERNAL_STORAGE";
    //QString write_permission = "android.permission.WRITE_EXTERNAL_STORAGE";

    QtAndroid::PermissionResult write_r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    QtAndroid::PermissionResult read_r = QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE");

    if(write_r == QtAndroid::PermissionResult::Denied || read_r == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync( QStringList() << "android.permission.WRITE_EXTERNAL_STORAGE" << "android.permission.READ_EXTERNAL_STORAGE", 10000);

        write_r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
        read_r = QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE");

        if(write_r == QtAndroid::PermissionResult::Denied || read_r == QtAndroid::PermissionResult::Denied)  {
            return false;
        }
    }

    return true;
#else
    return true;
#endif
#else
    return true;
#endif
}

bool Utility::requestBlePermission()
{

#ifdef Q_OS_ANDROID
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QtAndroid::PermissionResult coarse_location_r = QtAndroid::checkPermission("android.permission.ACCESS_COARSE_LOCATION");
    QtAndroid::PermissionResult fine_location_r = QtAndroid::checkPermission("android.permission.ACCESS_FINE_LOCATION");

    if(coarse_location_r == QtAndroid::PermissionResult::Denied || fine_location_r == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync( QStringList() << "android.permission.ACCESS_COARSE_LOCATION" << "android.permission.ACCESS_FINE_LOCATION", 10000);

        coarse_location_r = QtAndroid::checkPermission("android.permission.ACCESS_COARSE_LOCATION");
        fine_location_r = QtAndroid::checkPermission("android.permission.ACCESS_FINE_LOCATION");
        if(coarse_location_r == QtAndroid::PermissionResult::Denied || fine_location_r == QtAndroid::PermissionResult::Denied) {
            return false;
        }
    }

    return true;
#else
    return true;
#endif
#else
    return true;
#endif
}

QString Utility::aboutText()
{
    QString aboutText = tr(u8"ESC Tool %1<br>"
              "&copy; 深圳市弦动科技有限公司 2022. All rights reserved.<br>"
              "<a href=\"http://www.sinemotion.com/\">http://www.sinemotion.com/</a>").
            arg(QString::number(APP_VERSION, 'f', 2));
    return aboutText;
}

bool Utility::FloatEqual(float a, float b, float eps)
{
    return fabs(a - b) <= eps;
}

bool Utility::FloatLess(float a, float b, float eps)
{
    return a < (b - eps);
}
bool Utility::FloatLessEqual(float a, float b, float eps )
{
    return a < (b + eps);
}

bool Utility::FloatGreater(float a, float b, float eps)
{
    return a >  (b + eps);
}

bool Utility::FloatGreaterEqual(float a, float b, float eps) 
{
    return a > (b - eps);
}

float Utility::RoundFloat(float x) 
{
    return x < 0.0 ? qCeil(x - 0.5) : qFloor(x + 0.5);
}


CommPacktetData Utility::decodePacket(QByteArray& data)
{
    CommPacktetData packet_data;
    memset(&packet_data, 0, sizeof(CommPacktetData));

    packet_data.src_node_id = data[2]&0x7f;

    if(data[2]&0x80)
    {
        //服务帧
        packet_data.transfer_type = SERVICE_TRANFER;
    }
    else
    {
        if(packet_data.src_node_id == 0)
        {
            packet_data.transfer_type = ANONYMOUS_MESSAGE_TRANFER;
        }
        else
        {
            packet_data.transfer_type = MESSAGE_TRANFER;
        }
    }
    switch (packet_data.transfer_type) {
    case MESSAGE_TRANFER:
    {
        packet_data.data_type_id = (data[4]<<8) | data[3];
    }
        break;
    case ANONYMOUS_MESSAGE_TRANFER:
    {
        packet_data.data_type_id = data[3]&0x03;
    }
        break;
    case SERVICE_TRANFER:
    {
        packet_data.dst_node_id = data[3]&0x7f;
        packet_data.service_kind = ((data[3]&0x80) >>7);
        packet_data.data_type_id = data[4]&0x00ff;
    }
        break;
    default:
        break;
    }

    //数据长度
    quint16 data_len_h = ((quint8)data[5] >> 5)&0x07;
    quint16 data_len_l = (quint8)data[6];
    packet_data.data_len = (data_len_h << 8) | data_len_l;

    //优先级
    packet_data.priority = (quint8)data[5]&0x1f;

    return packet_data;
}

QByteArray Utility::encodePacket(CommPacktetData packet, QByteArray& data_field)
{
    QByteArray byte_array;
    byte_array.append(COMM_HEADER_1);
    byte_array.append(COMM_HEADER_2);

    quint8 data_len_l = packet.data_len&0x00FF;
    quint8 data_len_h = (packet.data_len&0xff00)>>8;

    switch (packet.transfer_type) {
    case MESSAGE_TRANFER:
    {
        byte_array.append(packet.src_node_id&0x7f);
        byte_array.append(packet.data_type_id&0x00ff);
        byte_array.append((packet.data_type_id&0xff00) >>8);
        byte_array.append( (data_len_h << 5) | packet.priority);
    }
        break;
    case ANONYMOUS_MESSAGE_TRANFER:
    {
        byte_array.append(packet.src_node_id&0x7f);
        byte_array.append(packet.data_type_id&0x0003);
        byte_array.append(packet.data_type_id&0x0);
        byte_array.append( (data_len_h << 5) | packet.priority);
    }
        break;
    case SERVICE_TRANFER:
    {
        byte_array.append(packet.src_node_id | 0x80);
        byte_array.append(packet.dst_node_id | 0x80);
        byte_array.append(packet.data_type_id & 0x00ff);
        byte_array.append( (data_len_h << 5) | packet.priority);
    }
        break;
    default:
        break;
    }
    byte_array.append(data_len_l);


    byte_array.append(data_field);
    quint16 packet_crc = calcCrc16(byte_array, 0, byte_array.length());

    byte_array.append(packet_crc&0x00ff);
    byte_array.append((packet_crc&0xff00) >> 8 );

    return byte_array;
}


quint16 Utility::calcSignatureCrc16(quint64 data_type_signature, QByteArray& data)
{
    quint16 crc = 0xffff;

    CustomByteArray signature;
    signature.appendData(data_type_signature);

    for (int i = 0; i < signature.length(); i++)
    {
        crc ^= (quint16)(signature[i]) << 8;

        for (int m = 0; m < 8; m++)
        {
            if ( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }

    for (int i = 0; i < data.length(); i++)
    {
        crc ^= (quint16)(data[i]) << 8;

        for (int m = 0; m < 8; m++)
        {
            if ( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }

    return crc;
}


/******************************************************************************
 * Name:    CRC-16-CCITT-FALSE x16+x12+x5+1
 * Poly:    0x1021
 * Init:    0xffff
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
quint16 Utility::calcCrc16(QByteArray& data, int start_index, int length)
{
    quint16 crc = 0xffff;
    int end_index = start_index + length;
    if(start_index > data.size() ||  end_index > data.size())
    {
        return crc;
    }

    for (int i = start_index; i < end_index; i++)
    {
        crc ^= (quint16)(data[i]) << 8;

        for (int m = 0; m < 8; m++)
        {
            if ( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }

    return crc;
}

void Utility::DelayMs(unsigned int msec)
{
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}


QByteArray Utility::HexStr2ByteArray(QString str)
{
    QByteArray byteArr;
    str = str.replace(" ",""); //去空格
    if (0 != (str.size() % 2)) {
        str.insert(str.size() - 1,'0'); //不是2的倍数 给最后一个补0 例如: a0 8 ,凑成 a0 08
    }

    for (int nIndex = 0; nIndex < str.size(); nIndex += 2) {
        char ch = QString(str[nIndex]).toInt(nullptr,16) * 16 + QString(str[nIndex + 1]).toInt(nullptr,16);
        byteArr.append(ch);
    }
    return byteArr;
}

QString Utility::ByteArray2HexStr(QByteArray byteArr)
{
    QString str = byteArr.toHex().toUpper();

    for (int nIndex = 3; nIndex < str.size();nIndex += 3) {
        str = str.insert(nIndex-1," ");
    }
    return str;
}
