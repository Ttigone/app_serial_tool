#pragma once
#include <qobject.h>

#include "CommDefine.h"

class DataPacket : public QObject
{
    Q_OBJECT
public:
    DataPacket();
    ~DataPacket();

    void processData(QByteArray data);

    void clearBuffer();
signals:
    void packetReceived(CommPacktetData packet_info, QByteArray &packet);
private:

    int checkDataPacket(QByteArray& pack_data);
private:
    QByteArray m_RecvBuffer;

    QByteArray m_EscRecvBuffer;

    int m_BoradState;
};

