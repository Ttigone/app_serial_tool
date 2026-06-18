#pragma once

#include <QObject>
#include "BleUart.h"
#include "DataPacket.h"
#include "Command.h"
#include "SerialPort.h"

#include "UpdateFirmware.h"

#include <QEventLoop>
#include <QTimer>

class QQmlEngine;
class QJSEngine;

class CommManager : public QObject {
  Q_OBJECT
 public:
  Q_DISABLE_COPY(CommManager)

  static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);
  static CommManager* singleton();

  CommManager(QObject* parent = nullptr);
  ~CommManager();

  Q_INVOKABLE Command* command();
  Q_INVOKABLE UpdateFirmware* firmwareUpdate();

  Q_INVOKABLE BleUart* bleUartDevice();
  Q_INVOKABLE SerialPort* serialPort();

  Q_INVOKABLE void emitBleDeviceConnected(QString addr, bool connected);

  Q_INVOKABLE void emitDeviceConnected(int device_id, bool connected);

  Q_INVOKABLE void initComm();

  Q_INVOKABLE QVariantMap scanEscDeive();

  Q_INVOKABLE void clearDataPacketBuffer();

 signals:
  void bleConnectChanged(QString addr, bool connected);

  void deviceConnected(int device_id, bool connected);

 private slots:
  void dataToSend(QByteArray& data);
  void dataRecv(QByteArray data);
  void parsePacketData(CommPacktetData packet_info, QByteArray& data);

 private:
  Command* m_Command;
  UpdateFirmware* m_FimwareUpdate;

  // 蓝牙
  BleUart* m_BleUart;

  // 串口
  SerialPort* m_SerialPort;

  // 包检验
  DataPacket* m_DataPacket;
};
