#include "BleUart.h"
#include "Utils/Utility.h"
#include "Utils/Config.h"

#include <QDebug>
#include <QLowEnergyConnectionParameters>
#include <QDateTime>

#include "CommManager.h"
BleUart::BleUart(QObject* parent) : QObject(parent) {
  m_Control = nullptr;
  m_Service = nullptr;
  m_UartServiceFound = false;
  m_ConnectDone = false;
  m_CurrentBleAddr = "";

#ifdef USE_OLD_BLE
  m_ServiceUuid = "0000ff10-0000-1000-8000-00805f9b34fb";
  mTxUuid = "0000ff12-0000-1000-8000-00805f9b34fb";
  mRxUuid = "0000ff11-0000-1000-8000-00805f9b34fb";
#else
  m_ServiceUuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
  mTxUuid = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";
  mRxUuid = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E ";
#endif

  m_Scanning = false;

  m_DeviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

  m_DeviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);

  connect(m_DeviceDiscoveryAgent,
          SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this,
          SLOT(addDevice(QBluetoothDeviceInfo)));
  connect(m_DeviceDiscoveryAgent,
          SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this,
          SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
  connect(m_DeviceDiscoveryAgent, SIGNAL(finished()), this,
          SLOT(scanFinished()));
}

BleUart::~BleUart() {
  if (m_Service) {
    delete m_Service;
    m_Service = nullptr;
  }

  if (m_Control) {
    delete m_Control;
    m_Control = nullptr;
  }
}

bool BleUart::startScan() {
  m_Scanning = true;
  m_ScanDevices.clear();

  if (m_DeviceDiscoveryAgent == NULL) {
    m_DeviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    m_DeviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    connect(m_DeviceDiscoveryAgent,
            SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this,
            SLOT(addDevice(QBluetoothDeviceInfo)));
    connect(m_DeviceDiscoveryAgent,
            SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this,
            SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_DeviceDiscoveryAgent, SIGNAL(finished()), this,
            SLOT(scanFinished()));
  }

  m_DeviceDiscoveryAgent->start(
      QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

  if (m_DeviceDiscoveryAgent->error() != 0) {
    qDebug() << "BleUart::startScan()" << m_DeviceDiscoveryAgent->error();
    m_DeviceDiscoveryAgent->stop();
    m_Scanning = false;

    disconnect(m_DeviceDiscoveryAgent,
               SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this,
               SLOT(addDevice(QBluetoothDeviceInfo)));
    disconnect(m_DeviceDiscoveryAgent,
               SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this,
               SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    disconnect(m_DeviceDiscoveryAgent, SIGNAL(finished()), this,
               SLOT(scanFinished()));

    delete m_DeviceDiscoveryAgent;

    m_DeviceDiscoveryAgent = NULL;

    return false;
  }

  emit scanDone(m_ScanDevices, false);

  return true;
}

bool BleUart::isScanning() { return m_Scanning; }

void BleUart::addDevice(QBluetoothDeviceInfo dev) {
  if ((dev.coreConfigurations() &
       QBluetoothDeviceInfo::LowEnergyCoreConfiguration)) {
    qDebug() << "BLE scan found device:" << dev.name()
             << "Valid:" << dev.isValid() << "Cached:" << dev.isCached()
             << "rssi:" << dev.rssi();
    QString name = dev.name();
#ifdef USE_OLD_BLE
    if (!m_ScanDevices.contains(dev.address().toString()))
#else
    if (name.contains("SINEMO") &&
        !m_ScanDevices.contains(dev.address().toString()))

#endif
    {
      QString addr = dev.address().toString();
      bool connected = (addr == m_CurrentBleAddr);
      QVariantMap temp_map;
      temp_map.insert("name", name);
      temp_map.insert("addr", addr);
      temp_map.insert("connected", connected);

      QString display_name = Config::singleton()->getBleName(addr, name);
      temp_map.insert("display_name", display_name);

      m_ScanDevices.insert(addr, temp_map);
      emit scanDone(m_ScanDevices, false);
    }
  }
}

void BleUart::scanFinished() {
  m_Scanning = false;
  emit scanDone(m_ScanDevices, true);
}

void BleUart::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error e) {
  qWarning() << "BLE Scan error: " << e;
  m_ScanDevices.clear();
  emit scanDone(m_ScanDevices, true);

  QString errorStr = "";

  switch (e) {
    case QBluetoothDeviceDiscoveryAgent::PoweredOffError:
      errorStr =
          QString(tr("The Bluetooth is powered off, Please power it on."));
      break;
    default:
      errorStr = QString(
          tr("The location service maybe turned off, Please turn it on."));
      break;
  }

  m_Scanning = false;
  emit bleScanError(errorStr);
}

void BleUart::startConnectBle(QString name, QString addr) {
  // 关闭ble扫描
  if (m_DeviceDiscoveryAgent->isActive()) {
    m_DeviceDiscoveryAgent->stop();
    m_Scanning = false;
    emit scanDone(m_ScanDevices, true);
  }

  if (m_Control) {
    m_Control->deleteLater();
    m_Control = nullptr;
  }

  if (m_Service) {
    m_Service->deleteLater();
    m_Service = nullptr;
  }

  m_UartServiceFound = false;

  m_ConnectDone = false;
  m_CurrentBleName = name;
  m_CurrentBleAddr = addr;

  m_Control = new QLowEnergyController(QBluetoothAddress(addr));

  // 发现了目标设备后触发的操作
  connect(m_Control, SIGNAL(serviceDiscovered(QBluetoothUuid)), this,
          SLOT(serviceDiscovered(QBluetoothUuid)));

  // 配置服务
  connect(m_Control, SIGNAL(discoveryFinished()), this,
          SLOT(serviceDiscoveryFinished()));

  // 错误连接
  connect(m_Control, SIGNAL(error(QLowEnergyController::Error)), this,
          SLOT(controllerError(QLowEnergyController::Error)));

  // 成功连接
  connect(m_Control, SIGNAL(connected()), this, SLOT(deviceConnected()));

  // 断开连接
  connect(m_Control, SIGNAL(disconnected()), this, SLOT(deviceDisconnected()));

  // 连接状态变化
  connect(m_Control,
          SIGNAL(stateChanged(QLowEnergyController::ControllerState)), this,
          SLOT(controlStateChanged(QLowEnergyController::ControllerState)));
  m_Control->connectToDevice();
}

void BleUart::disconnectBle() {
  qDebug() << "disconnectBle!";

  if (m_Control) {
    m_Control->deleteLater();
    m_Control = nullptr;
  }

  if (m_Service) {
    m_Service->deleteLater();
    m_Service = nullptr;
  }

  m_ConnectDone = false;
  m_CurrentBleAddr = "";

  emit bleConnectChanged("", "", false);
  // CommManager::singleton()->emitBleDeviceConnected("", false);
}

bool BleUart::isConnected() { return m_Control != nullptr && m_ConnectDone; }

bool BleUart::isConnecting() { return m_Control && !m_ConnectDone; }

void BleUart::writeData(QByteArray data) {
  if (isConnected()) {
    QDateTime time = QDateTime::currentDateTime();
    qDebug() << time.toString("yyyy-MM-dd hh:mm:ss.zzz") << "BleUart Write"
             << Utility::ByteArray2HexStr(data);

    const QLowEnergyCharacteristic rxChar =
        m_Service->characteristic(QBluetoothUuid(QUuid(mRxUuid)));
    if (rxChar.isValid()) {
#ifdef USE_OLD_BLE
      int chunk = 20;
#else
      int chunk = 200;
#endif
      while (data.size() > chunk) {
        m_Service->writeCharacteristic(rxChar, data.mid(0, chunk),
                                       QLowEnergyService::WriteWithoutResponse);
        data.remove(0, chunk);
      }
      m_Service->writeCharacteristic(rxChar, data,
                                     QLowEnergyService::WriteWithoutResponse);
    }
  }
}

void BleUart::serviceDiscovered(QBluetoothUuid uuid) {
  if (uuid == QBluetoothUuid(QUuid(m_ServiceUuid))) {
    qDebug() << "BLE UART service found!";
    m_UartServiceFound = true;
  }

  qDebug() << "serviceDiscovered";
}

void BleUart::serviceDiscoveryFinished() {
  if (m_Service) {
    m_Service->deleteLater();
    m_Service = nullptr;
  }

  if (m_UartServiceFound) {
    qDebug() << "Connecting to BLE UART service";
    m_Service = m_Control->createServiceObject(
        QBluetoothUuid(QUuid(m_ServiceUuid)), this);

    // 服务状态值发生改变
    connect(m_Service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
            this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));
    // 特征值发生改变
    connect(m_Service,
            SIGNAL(characteristicChanged(QLowEnergyCharacteristic, QByteArray)),
            this, SLOT(updateData(QLowEnergyCharacteristic, QByteArray)));

    connect(m_Service,
            SIGNAL(descriptorWritten(QLowEnergyDescriptor, QByteArray)), this,
            SLOT(confirmedDescriptorWrite(QLowEnergyDescriptor, QByteArray)));

    m_Service->discoverDetails();
  } else {
    qWarning() << "BLE UART service not found";
    disconnectBle();
  }
}

void BleUart::controllerError(QLowEnergyController::Error e) {
  qWarning() << "BLE error:" << e;
  disconnectBle();
  emit bleError(tr("BLE error: ") + Utility::QEnumToQString(e));
}

void BleUart::deviceConnected() {
  qDebug() << "BLE device connected";
  m_Control->discoverServices();
}

void BleUart::deviceDisconnected() {
  qDebug() << "BLE service disconnected";
  disconnectBle();
}

void BleUart::controlStateChanged(QLowEnergyController::ControllerState state) {
  (void)state;

  qDebug() << "controlStateChanged" << state;
}

void BleUart::serviceStateChanged(QLowEnergyService::ServiceState s) {
  // A descriptor can only be written if the service is in the ServiceDiscovered
  // state
  switch (s) {
    case QLowEnergyService::ServiceDiscovered: {
      // looking for the TX characteristic
      const QLowEnergyCharacteristic txChar =
          m_Service->characteristic(QBluetoothUuid(QUuid(mTxUuid)));

      if (!txChar.isValid()) {
        qDebug() << "BLE Tx characteristic not found";
        break;
      }

      // looking for the RX characteristic
      const QLowEnergyCharacteristic rxChar =
          m_Service->characteristic(QBluetoothUuid(QUuid(mRxUuid)));

      if (!rxChar.isValid()) {
        qDebug() << "BLE Rx characteristic not found";
        break;
      }

      // Bluetooth LE spec Where a characteristic can be notified, a Client
      // Characteristic Configuration descriptor shall be included in that
      // characteristic as required by the Bluetooth Core Specification Tx
      // notify is enabled
      m_NotificationDescTx =
          txChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

      qDebug() << "m_NotificationDescTx"
               << m_NotificationDescTx.uuid().toString();

      if (m_NotificationDescTx.isValid()) {
        // enable notification
        m_Service->writeDescriptor(m_NotificationDescTx,
                                   QByteArray::fromHex("0100"));
      }

      break;
    }

    default:
      break;
  }
}

void BleUart::updateData(const QLowEnergyCharacteristic& c,
                         const QByteArray& value) {
  if (c.uuid() == QBluetoothUuid(QUuid(mTxUuid))) {
    QDateTime time = QDateTime::currentDateTime();
    qDebug() << time.toString("yyyy-MM-dd hh:mm:ss.zzz") << "Recv"
             << Utility::ByteArray2HexStr(value);

    emit dataRx(value);
  }
}

void BleUart::confirmedDescriptorWrite(const QLowEnergyDescriptor& d,
                                       const QByteArray& value) {
  qDebug() << "confirmedDescriptorWrite" << Utility::ByteArray2HexStr(value);

  if (d.isValid() && d == m_NotificationDescTx && value == QByteArray("0000")) {
    // disabled notifications -> assume disconnect intent
    disconnectBle();
  } else {
    m_ConnectDone = true;
    emit bleConnectChanged(m_CurrentBleName, m_CurrentBleAddr, true);

    // CommManager::singleton()->emitBleDeviceConnected(m_CurrentBleAddr,
    // m_ConnectDone);
  }
}
