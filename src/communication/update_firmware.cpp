#include "UpdateFirmware.h"
#include <QDebug>
#include "Utils/CustomByteArray.h"
#include "Utils/Utility.h"

#include "CommDefine.h"
#include "Data/ParamManager.h"

UpdateFirmware::UpdateFirmware(QObject* parent) : QObject(parent) {
  m_File = new QFile(this);
}

UpdateFirmware::~UpdateFirmware() {}

int UpdateFirmware::restartESC() {
  int ret = 0;
  quint8 esc_id = ParamManager::singleton()->getEscID();

  if (sendAppRestart(esc_id)) {
    if (m_RestartOK) {
      ret = 0;
    } else {
      ret = 1;
    }
  } else {
    ret = -1;
  }

  return ret;
}

bool UpdateFirmware::checkFile(QString product_id) {
  m_File->read((char*)m_FileBuffer, 32);

  QByteArray head_byte((char*)m_FileBuffer, 32);
  CustomByteArray data_array(head_byte);
  m_HardwareID = data_array.popFrontUint16();
  m_AppVersion = data_array.popFrontUint16();

  m_FileSizeKb = data_array.popFrontUint16();
  m_FileCrc = data_array.popFrontUint16();

  m_HardwareIDStr =
      QString("V%1.%2")
          .arg((m_HardwareID >> 8) & 0xff, 2, 10, QLatin1Char('0'))
          .arg(m_HardwareID & 0xff, 2, 10, QLatin1Char('0'));
  m_SoftwareVersionStr =
      QString("V%1.%2")
          .arg((m_AppVersion >> 8) & 0xff, 2, 10, QLatin1Char('0'))
          .arg(m_AppVersion & 0xff, 2, 10, QLatin1Char('0'));

  return true;
}

void UpdateFirmware::updateFile(QString file_name, int device_id) {
  int dst_device_id = device_id;

#ifdef Android_Platform
  if (file_name.startsWith("file:/")) {
    file_name.remove(0, 6);
  }
#else
  if (file_name.startsWith("file:///")) {
    file_name.remove(0, 8);
  } else if (file_name.startsWith("file:")) {
    file_name.remove(0, 5);
  }
#endif

  m_File->setFileName(file_name);

  if (m_File->open(QFile::ReadOnly) == false) {
    emit updateMessage(
        tr("<font color=#e81123>Open firmware file error...</font>"));
    return;
  }

  if (checkFile("0x01") == false) {
    emit updateMessage(
        tr("<font color=#e81123>Firmware file check error...</font>"));
    m_File->close();
    return;
  }

  emit updateMessage(tr("Firmware file check pass..."));

  QFileInfo file_info(*m_File);

  quint32 file_size = file_info.size();
  if ((file_size - 32) % 256 != 0 || (file_size - 32) != m_FileSizeKb * 1024) {
    emit updateMessage(
        tr("<font color=#e81123>Firmware file size error...</font>"));
    m_File->close();
    return;
  } else {
    m_PakcetMaxIndex = file_size / 256;
  }

  int progress = 0;

  if (sendAppRestart(dst_device_id) == false) {
    // 只有bootloader的情况,则需要用户自己知道现在处于bootloader状态
    emit updateMessage(
        tr("<font color=#e81123>Send restart cmd failure...</font>"));
    m_File->close();
    return;
  }

  if (m_RestartOK == false) {
    emit updateMessage(tr("<font color=#e81123>Restart failure...</font>"));
    m_File->close();
    return;
  }

  emit updateMessage(tr("Restart success..."));

  if (sendHoldBootloader(dst_device_id) == false) {
    emit updateMessage(tr(
        "<font color=#e81123>Send hold on bootloader cmd failure...</font>"));
    m_File->close();
    return;
  }

  if (sendStartTranmit(dst_device_id) == false) {
    emit updateMessage(
        tr("<font color=#e81123>Send start cmd failure...</font>"));
    m_File->close();
    return;
  }

  if (m_StartUpdateState == 1) {
    emit updateMessage(
        tr("<font color=#e81123>Device Check Firmware File Error...</font>"));
    m_File->close();
    return;
  }

  emit updateMessage(tr("Start tranmit..."));

  m_PacketIndex = 0;
  m_PacketNextIndex = 1;
  m_ErrorCount = 0;

  while (m_PacketIndex < m_PakcetMaxIndex) {
    m_File->seek(32 + m_PacketIndex * UPDATE_PACKET_SIZE);
    m_File->read((char*)m_FileBuffer, UPDATE_PACKET_SIZE);

    if (sendTranFirmware(dst_device_id, m_PacketIndex, m_FileBuffer,
                         UPDATE_PACKET_SIZE) == false) {
      emit updateMessage(
          tr("<font color=#e81123>Tranmit file data timeout...</font>"));
      m_File->close();
      return;
    }
    if (m_PacketNextIndex == m_PacketIndex) {
      m_ErrorCount = 0;
    } else {
      m_ErrorCount++;
    }

    m_PacketNextIndex = m_PacketIndex + 1;
    if (m_ErrorCount > 10) {
      emit updateMessage(
          tr("<font color=#e81123>Tranmit file data error...</font>"));
      m_File->close();
      return;
    }

    progress = (int)(m_PacketIndex * 100 / m_PakcetMaxIndex);

    emit updateProcess(progress);
  }

  if (sendEndTranmit(dst_device_id) == false) {
    emit updateMessage(
        tr("<font color=#e81123>Send end cmd failure...</font>"));
    m_File->close();
    return;
  }

  if (m_EndUpdateState == 1) {
    emit updateMessage(tr("<font color=#e81123>Update CRC error...</font>"));
  } else {
    emit updateMessage(tr("<font color=#0066B4>Update Success...</font>"));
  }

  m_File->close();
}

void UpdateFirmware::parsePacket(CommPacktetData packet_info,
                                 QByteArray& data_field) {
  CustomByteArray data_array(data_field);

  switch (packet_info.data_type_id) {
    case COMM_RESTART: {
      m_RestartOK = data_array.popFrontUint8();
    } break;
    case COMM_HOLD_BOOTLOADER: {
      m_HoldBootloader = data_array.popFrontUint8();
    } break;
    case COMM_START_UPDATE: {
      m_StartUpdateState = data_array.popFrontUint8();
    } break;
    case COMM_TRAN_FIRMWARE: {
      m_RecvPacketState = data_array.popFrontUint8();
      m_RecvPacketIndex = data_array.popFrontUint16();
      if (m_RecvPacketIndex >= 0 && m_RecvPacketIndex <= m_PakcetMaxIndex) {
        m_PacketIndex = m_RecvPacketIndex;
      }
    } break;
    case COMM_END_UPDATE: {
      m_EndUpdateState = data_array.popFrontUint8();
    } break;
    default:
      break;
  }

  emit sendCmdFinished(packet_info.data_type_id);
}

bool UpdateFirmware::sendAppRestart(int device_id) {
  // uint40 MAGIC_NUMBER = 0xACCE551B1E

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = device_id;
  packet.data_type_id = COMM_RESTART;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  quint32 magic_num_l = 0xCE551B1E;
  quint8 macig_num_h = 0xAC;
  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint32(magic_num_l);
  data_field.appendUint8(macig_num_h);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);
  m_RestartOK = false;

  bool ret = sendCmd(cmd, packet.data_type_id);
  return ret;
}

bool UpdateFirmware::sendBootloaderRestart(int device_id) {
  // uint40 MAGIC_NUMBER = 0xACCE551B1E

  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = device_id;
  packet.data_type_id = COMM_RESTART;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  quint32 magic_num_l = 0xCE551B1E;
  quint8 macig_num_h = 0xAC;
  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint32(magic_num_l);
  data_field.appendUint8(macig_num_h);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  m_RestartOK = false;
  bool ret = sendCmd(cmd, packet.data_type_id);
  return ret;
}

bool UpdateFirmware::sendHoldBootloader(int device_id) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = device_id;
  packet.data_type_id = COMM_HOLD_BOOTLOADER;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  bool ret = sendCmd(cmd, packet.data_type_id);
  return ret;
}

bool UpdateFirmware::sendStartTranmit(int device_id) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = device_id;
  packet.data_type_id = COMM_START_UPDATE;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();
  data_field.appendUint16(m_HardwareID);
  data_field.appendUint16(m_FileSizeKb);
  data_field.appendUint16(m_FileCrc);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  bool ret = sendCmd(cmd, packet.data_type_id);
  return ret;
}

bool UpdateFirmware::sendTranFirmware(int device_id, quint16 packet_index,
                                      quint8* buffer, quint32 buffer_len) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = device_id;
  packet.data_type_id = COMM_TRAN_FIRMWARE;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();

  data_field.appendUint16(packet_index);
  data_field.append((char*)buffer, buffer_len);

  // 添加签名CRC,到包头
  quint16 data_crc = Utility::calcSignatureCrc16(
      SINEMOTION_TRANSFIRMWARE_SIGNATURE, data_field);
  data_field.push_front((data_crc & 0xff00) >> 8);
  data_field.push_front(data_crc & 0x00ff);

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

#ifdef Android_Platform
  bool ret = sendCmd(cmd, packet.data_type_id, 40);
#else
  bool ret = sendCmd(cmd, packet.data_type_id);
#endif
  return ret;
}

bool UpdateFirmware::sendEndTranmit(int device_id) {
  CommPacktetData packet;
  packet.src_node_id = DEVICE_PC_NODE;
  packet.dst_node_id = device_id;
  packet.data_type_id = COMM_END_UPDATE;
  packet.transfer_type = SERVICE_TRANFER;
  packet.service_kind = 1;
  packet.priority = 0;

  CustomByteArray data_field;
  data_field.clear();

  packet.data_len = data_field.length();

  QByteArray cmd = Utility::encodePacket(packet, data_field);

  bool ret = sendCmd(cmd, packet.data_type_id);
  return ret;
}

bool UpdateFirmware::sendCmd(QByteArray& cmd, quint16 cmd_id, quint16 timeout) {
  bool ret = false;
  quint16 used_cmd = cmd_id;
  int retry_cnt = 0;

  auto conn = connect(this, &UpdateFirmware::sendCmdFinished,
                      [&ret, &used_cmd](quint16 recv_cmd) {
                        if (recv_cmd == used_cmd) {
                          ret = true;
                        }
                      });

  while (retry_cnt++ < 20) {
    emit sendData(cmd);

    for (int i = 0; i < timeout; i++) {
      Utility::DelayMs(10);
      if (ret) {
        break;
      }
    }

    if (ret) {
      break;
    }
  }

  disconnect(conn);
  return ret;
}
