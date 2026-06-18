/*
    Copyright 2017 Benjamin Vedder	benjamin@vedder.se

    This file is part of VESC Tool.

    VESC Tool is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VESC Tool is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#ifndef BLEUART_H
#define BLEUART_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QVariantMap>

class BleUart : public QObject {
  Q_OBJECT
 public:
  explicit BleUart(QObject* parent = nullptr);
  ~BleUart();

  Q_INVOKABLE bool startScan();
  Q_INVOKABLE bool isScanning();

  Q_INVOKABLE void startConnectBle(QString name, QString addr);
  Q_INVOKABLE void disconnectBle();
  Q_INVOKABLE bool isConnected();
  Q_INVOKABLE bool isConnecting();

 signals:
  void dataRx(QByteArray data);
  void scanDone(QVariantMap devs, bool done);

  void bleConnectChanged(QString ble_name, QString ble_addr, bool flag);
  void bleScanError(QString scan_error);
  void bleError(QString info);

 public slots:
  void writeData(QByteArray data);

 private slots:
  void addDevice(QBluetoothDeviceInfo dev);
  void scanFinished();
  void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error e);

  void serviceDiscovered(QBluetoothUuid uuid);
  void serviceDiscoveryFinished();
  void controllerError(QLowEnergyController::Error);
  void deviceConnected();
  void deviceDisconnected();
  void controlStateChanged(QLowEnergyController::ControllerState state);

  void serviceStateChanged(QLowEnergyService::ServiceState s);
  void updateData(const QLowEnergyCharacteristic& c, const QByteArray& value);
  void confirmedDescriptorWrite(const QLowEnergyDescriptor& d,
                                const QByteArray& value);

 private:
  QBluetoothDeviceDiscoveryAgent* m_DeviceDiscoveryAgent;

  QString m_CurrentBleName;
  QString m_CurrentBleAddr;
  QLowEnergyController* m_Control;
  QLowEnergyService* m_Service;
  bool m_UartServiceFound;
  QString m_ServiceUuid;
  bool m_ConnectDone;
  bool m_Scanning;

  QLowEnergyDescriptor m_NotificationDescTx;
  QString mRxUuid;
  QString mTxUuid;

  QVariantMap m_ScanDevices;
};

#endif  // BLEUART_H
