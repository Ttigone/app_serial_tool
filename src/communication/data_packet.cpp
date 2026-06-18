#include "DataPacket.h"
#include <QDebug>
#include "Utils/CustomByteArray.h"
#include "Utils/Utility.h"
#include "Data/ParamManager.h"
#include "CommManager.h"
#include "Logger/Logger.h"



DataPacket::DataPacket()
{
}

DataPacket::~DataPacket()
{
}

void DataPacket::processData(QByteArray data)
{
    //从接收缓冲区中读取数据
    int pack_len = 0;
    int buffer_index = 0;

    m_RecvBuffer.append(data);

    for (int i = 0; i < m_RecvBuffer.size(); )
    {
        QByteArray sub_buffer = m_RecvBuffer.mid(i);

        pack_len = checkDataPacket(sub_buffer);
        if (pack_len > 0)
        {
            QByteArray pack_buffer = m_RecvBuffer.mid(i, pack_len);

            //qDebug()<<"DataPacket::processData" << Utility::ByteArray2HexStr(pack_buffer);

            CommPacktetData packet_info = Utility::decodePacket(pack_buffer);
            if(packet_info.service_kind == 0)
            {
                //qDebug()<<"DataPacket::Reponse" << Utility::ByteArray2HexStr(pack_buffer);
                QByteArray data_field = pack_buffer.mid(7, packet_info.data_len);
                emit packetReceived(packet_info, data_field);
            }

            i += pack_len;
            buffer_index = i;
        }
        else
        {
            i++;
        }
    }

    m_RecvBuffer = m_RecvBuffer.mid(buffer_index);
    if (m_RecvBuffer.size() > 1024)
    {
        LOG_DEBUG << "Clear m_RecvBuffer";
        m_RecvBuffer.clear();
    }
}

void DataPacket::clearBuffer()
{
    m_RecvBuffer.clear();
    m_EscRecvBuffer.clear();
}

int DataPacket::checkDataPacket(QByteArray& pack_data)
{
    int pack_len = 0;
    const quint8 header_1 = COMM_HEADER_1;
    const quint8 header_2 = COMM_HEADER_2;

    if (pack_data.size() < 9)
    {
        return pack_len;
    }

    if ((quint8)pack_data[0] != header_1 || (quint8)pack_data[1] != header_2)
        return pack_len;

    //解析包长度
    quint8 data_len_h = (quint8)pack_data[5] >> 5;
    quint8 data_len_l = (quint8)pack_data[6];
    quint16 data_len = (data_len_h << 8) | data_len_l;
    quint16 parse_len = 0;
    parse_len += 7;
    parse_len += data_len;
    parse_len += 2; //校验和

    if (parse_len > pack_data.size() || parse_len > 512)
    {
        return pack_len;
    }

    quint16 check_crc = ((quint8)pack_data[parse_len - 1] << 8) | ((quint8)pack_data[parse_len - 2]);
    quint16 calc_check_crc = Utility::calcCrc16(pack_data, 0, parse_len - 2);
    if (check_crc == calc_check_crc)
    {
        pack_len = parse_len;
        return pack_len;
    }
    else
    {
        return pack_len;
    }

    return pack_len;
}
