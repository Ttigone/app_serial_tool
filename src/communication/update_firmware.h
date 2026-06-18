#pragma once
#include <QObject>
#include "DataPacket.h"

#include "Data/ParamManager.h"
#include <QFile>
#include <QFileInfo>
#include <QString>

#define UPDATE_PACKET_SIZE (256)

class UpdateFirmware : public QObject {
  Q_OBJECT
 public:
  UpdateFirmware(QObject* parent = nullptr);
  ~UpdateFirmware();

  Q_INVOKABLE int restartESC();

  Q_INVOKABLE bool checkFile(QString product_id);

  Q_INVOKABLE void updateFile(QString file, int device_id);

  void parsePacket(CommPacktetData packet_info, QByteArray& data_field);
 signals:
  void updateProcess(int progeress);
  void updateMessage(QString message);

  void sendData(QByteArray& data);
  void sendCmdFinished(quint16 cmd);

 public slots:

 private:
  bool sendAppRestart(int device_id);
  bool sendBootloaderRestart(int device_id);

  bool sendHoldBootloader(int device_id);
  bool sendStartTranmit(int device_id);
  bool sendTranFirmware(int device_id, quint16 packet_index, quint8* buffer,
                        quint32 buffer_len);
  bool sendEndTranmit(int device_id);

  bool sendCmd(QByteArray& cmd, quint16 cmd_id, quint16 time_out = 15);

  void parseFirmwareInfo(QByteArray data);

  QFile* m_File;
  quint8 m_FileBuffer[UPDATE_PACKET_SIZE];
  QString m_HardwareIDStr;
  QString m_SoftwareVersionStr;
  quint16 m_HardwareID;
  quint16 m_AppVersion;
  quint32 m_FileSizeKb;
  quint16 m_FileCrc;

  quint8 m_RestartOK;

  quint8 m_HoldBootloader;

  quint8 m_StartUpdateState;

  quint16 m_PacketIndex;
  quint16 m_PacketNextIndex;
  quint16 m_ErrorCount;

  quint16 m_PakcetMaxIndex;
  quint16 m_RecvPacketIndex;
  quint8 m_RecvPacketState;

  quint8 m_EndUpdateState;
};
