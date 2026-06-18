#include "CommManager.h"
#include <QDebug>
#include "Utils/Utility.h"
#include "CommDefine.h"
#include "Data/ParamManager.h"

#include "Public/AppInterface.h"
#include "Utils/Config.h"

QObject* CommManager::qmlSingleton(QQmlEngine* engine,
                                   QJSEngine* scriptEngine) {
  Q_UNUSED(engine);
  Q_UNUSED(scriptEngine);
  static CommManager* _singleton = nullptr;
  if (!_singleton) _singleton = new CommManager;
  return _singleton;
}

CommManager* CommManager::singleton() {
  return qobject_cast<CommManager*>(qmlSingleton(nullptr, nullptr));
}

CommManager::CommManager(QObject* parent) : QObject(parent) {
  m_BleUart = new BleUart(this);
  connect(m_BleUart, SIGNAL(dataRx(QByteArray)), this,
          SLOT(dataRecv(QByteArray)));

  m_SerialPort = new SerialPort(this);
  connect(m_SerialPort, SIGNAL(dataRx(QByteArray)), this,
          SLOT(dataRecv(QByteArray)));

  m_DataPacket = new DataPacket();
  m_Command = new Command();
  connect(m_Command, &Command::sendData, this, &CommManager::dataToSend);

  m_FimwareUpdate = new UpdateFirmware();
  connect(m_FimwareUpdate, &UpdateFirmware::sendData, this,
          &CommManager::dataToSend);

  connect(m_DataPacket, &DataPacket::packetReceived, this,
          &CommManager::parsePacketData);
}

CommManager::~CommManager() {}

Command* CommManager::command() { return m_Command; }

UpdateFirmware* CommManager::firmwareUpdate() { return m_FimwareUpdate; }

BleUart* CommManager::bleUartDevice() { return m_BleUart; }

SerialPort* CommManager::serialPort() { return m_SerialPort; }

void CommManager::emitBleDeviceConnected(QString addr, bool connected) {
  emit bleConnectChanged(addr, connected);
}

void CommManager::emitDeviceConnected(int device_id, bool connected) {
  emit deviceConnected(device_id, connected);
}

void CommManager::initComm() {
  if (!m_Command->queryNodeInfo()) {
    // emit deviceConnected(DEVICE_ESC, false);
    // emit deviceConnected(DEVICE_REMOTE_SENDER, false);

    AppInterface::singleton()->emitMessageDialog(
        tr("Ble Error"), tr("Unknown Ble Device"), false);
    return;
  }
  // quint8 device_id = ParamManager::singleton()->getDeviceID();
  // emit deviceConnected(device_id, true);
}

QVariantMap CommManager::scanEscDeive() {
  int start_id = Config::singleton()->getEscScanStartID();
  int end_id = Config::singleton()->getEscScanEndID();

  m_Command->scanEscActive(start_id, end_id);
  QVariantMap esc_map = m_Command->getNodeActiveList();

  return esc_map;
}

void CommManager::clearDataPacketBuffer() { m_DataPacket->clearBuffer(); }

void CommManager::dataToSend(QByteArray& data) {
#ifdef HAS_BLUETOOTH
  if (m_BleUart->isConnected()) {
    m_BleUart->writeData(data);
  }
#endif

#ifdef HAS_SERIAL
  if (m_SerialPort->isConnected()) {
    m_SerialPort->writeData(data);
  }
#endif
}

void CommManager::dataRecv(QByteArray data) { m_DataPacket->processData(data); }

void CommManager::parsePacketData(CommPacktetData packet_info,
                                  QByteArray& data_field) {
  switch (packet_info.data_type_id) {
    case COMM_RESTART:
    case COMM_HOLD_BOOTLOADER:
    case COMM_START_UPDATE:
    case COMM_TRAN_FIRMWARE:
    case COMM_END_UPDATE: {
      m_FimwareUpdate->parsePacket(packet_info, data_field);
    } break;
    default:
      m_Command->parsePacket(packet_info, data_field);
      break;
  }
}
