import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

import FluentUI 1.0

FluPopup {
    id: ble

    property int pullHeight: 64 //下拉刷新

    signal startConnect(string ble_name, string ble_addr)

    function scanBleDevice() {
        console.log(deviceListView.height);
        if (!BleUart.isScanning()) {
            console.log("start scan ble");
            if (BleUart.startScan() === false) {
                close();
            }
        }

        ble_scan_busy.visible = true;
    }

    closePolicy: Popup.CloseOnPressOutside
    implicitHeight: 600
    implicitWidth: 400

    onClosed: {}
    onOpened: {
        scanBleDevice();
    }

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            id: header

            Layout.fillWidth: true
            color: FluTheme.primaryColor.light
            implicitHeight: 50

            FluText {
                id: page_label

                font.pixelSize: 18
                text: qsTr("Avaliable Devices")

                anchors {
                    horizontalCenter: header.horizontalCenter
                    verticalCenter: header.verticalCenter
                }
            }
            FluProgressRing {
                id: ble_scan_busy

                indeterminate: true

                anchors {
                    margins: 5
                    right: header.right
                    verticalCenter: header.verticalCenter
                }
            }
        }
        ListModel {
            id: bleItemsModel

        }
        ListView {
            id: deviceListView

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 10
            clip: true
            delegate: availableDelegate
            model: bleItemsModel
            spacing: 5

            states: [
                State {
                    id: downRefresh

                    name: "downRefresh"
                    when: (deviceListView.contentY < -pullHeight)

                    StateChangeScript {
                        name: "scanBleDevice"
                        script: scanBleDevice()
                    }
                }
            ]

            Component {
                id: availableDelegate

                Rectangle {
                    id: bleItem

                    height: 60
                    width: deviceListView.width

                    FluText {
                        id: device_name_text

                        font.pixelSize: 16
                        text: ble_name

                        anchors {
                            margins: 5
                            top: bleItem.top
                        }
                    }
                    FluText {
                        id: device_id_text

                        font.pixelSize: 12
                        text: ble_addr

                        anchors {
                            bottom: bleItem.bottom
                            margins: 5
                        }
                    }
                    MouseArea {
                        anchors.fill: bleItem

                        onClicked: {
                            deviceListView.currentIndex = index;
                            BleUart.startConnectBle(ble_name, ble_addr);

                            close();

                            startConnect(ble_name, ble_addr);
                            console.log("ble_addr" + ble_addr);
                            console.log("View " + deviceListView.currentIndex);
                            console.log("Current Item" + index);
                        }
                    }
                }
            }
        }
    }
    Connections {
        target: BleUart

        onScanDone: {
            if (done) {
                console.log("scan ble finished");
                ble_scan_busy.visible = false;
            }

            bleItemsModel.clear();

            for (var addr in devs) {
                var data_map = devs[addr];
                bleItemsModel.append({
                    ble_name: data_map['name'],
                    ble_addr: data_map['addr']
                });
            }
        }
    }
}
