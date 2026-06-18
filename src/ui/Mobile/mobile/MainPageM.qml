import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.0

import FluentUI 1.0

Rectangle {
    id: page

    property int comBoxcEscIndex: 0
    property var escDeviceDisplay: []
    property var escDevices: []
    property int escNodeID: 0
    property int item_font_size: 14
    property int item_height: 70
    property int item_icon_button_size: 60
    property int item_icon_size: 50
    property int item_width: 120

    function refreshEscList() {
        CommManager.scanEscDeive();
    }

    Component.onCompleted: {}

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: 5
        width: parent.width

        Rectangle {
            id: header

            Layout.fillWidth: true
            color: FluTheme.primaryColor.dark
            implicitHeight: 50

            FluText {
                id: title_label

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: FluColors.White
                font.pixelSize: 24
                text: qsTr("SINEMOTION")
            }
        }
        RowLayout {
            Layout.margins: 10
            Layout.preferredWidth: page.width
            spacing: 30

            Item {
                Layout.alignment: Qt.AlignHCenter
                height: item_height
                width: item_width

                FluIconButton {
                    id: connectButton

                    //iconSource:FluentIcons.DisconnectDrive
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.primaryColor.light
                    height: item_icon_button_size
                    iconColor: FluColors.White
                    iconSize: item_icon_size
                    iconSource: FluentIcons.MapDrive
                    width: item_icon_button_size

                    onClicked: {
                        if (Utility.requestBlePermission()) {
                            if (BleUart.isConnected()) {
                                BleUart.disconnectBle();
                            } else {
                                bluetoothDialog.open();
                            }
                        } else {
                            showMessageDialog(qsTr("Error"), qsTr("Unable to scan bluetooth deive"));
                        }
                    }

                    BluetoothDialogM {
                        id: bluetoothDialog

                        onStartConnect: {
                            showLoadingDialog(qsTr("Connect Bluetooth..."));
                        }
                    }
                }
                FluText {
                    id: connectButtonText

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: connectButton.bottom
                    font.pixelSize: item_font_size
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Connect")
                    width: parent.width
                    wrapMode: Text.WrapAnywhere
                }
            }
            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter

                FluText {
                    id: bluetooothNameLabel

                    horizontalAlignment: Text.AlignLeft
                    text: qsTr("Bluetooth : ")
                    wrapMode: Text.WrapAnywhere
                }
                RowLayout {
                    spacing: 5

                    FluText {
                        text: qsTr("ESC:")
                    }
                    FluComboBox {
                        id: escComBox

                        Layout.preferredWidth: 80
                        model: []

                        onActivated: {
                            if (currentIndex != -1) {
                                escNodeIDText.text = escDevices[escComBox.currentIndex];
                                ParamManager.setEscID(escDevices[escComBox.currentIndex]);
                            }
                        }
                        onCurrentIndexChanged: {
                            if (currentIndex == -1) {
                                escNodeIDText.text = 32;
                                ParamManager.setEscID(32);
                            } else {
                                escNodeIDText.text = escDevices[escComBox.currentIndex];
                                ParamManager.setEscID(escDevices[escComBox.currentIndex]);
                            }
                        }
                    }
                    FluButton {
                        id: scanButton

                        font.family: "FontAwesome"
                        text: "\uf002"
                        width: height

                        onClicked: {
                            if (!BleUart.isConnected()) {
                                showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                                return;
                            }

                            showLoadingDialog(qsTr("Scan ESC Device..."));
                            refreshEscList();
                            hideLoadingDialog();

                            if (escDevices.length === 0) {
                                showMessageDialog(qsTr("Scan ESC Device"), qsTr("ESC Device Not Found, Use Default Node ID(32)"));
                            }
                        }
                    }
                }
                RowLayout {
                    spacing: 5

                    FluText {
                        text: qsTr("ESC Node ID:")
                    }
                    FluText {
                        id: escNodeIDText

                        text: "32"
                    }
                }
            }
            Item {
                Layout.fillWidth: true
            }
        }
        Item {
            height: 100
        }
        GridLayout {
            Layout.fillHeight: true
            Layout.margins: 10
            Layout.preferredWidth: page.width
            columnSpacing: 30
            columns: 2
            rowSpacing: 30

            Item {
                Layout.alignment: Qt.AlignHCenter
                height: item_height
                width: item_width

                FluIconButton {
                    id: escSettingButton

                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.primaryColor.light
                    height: item_icon_button_size
                    iconColor: FluColors.White
                    iconSize: item_icon_size
                    iconSource: FluentIcons.Devices
                    width: item_icon_button_size

                    onClicked: {
                        showPage(1);
                    }
                }
                FluText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: escSettingButton.bottom
                    font.pixelSize: item_font_size
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("ESC Setting")
                    width: parent.width
                    wrapMode: Text.WrapAnywhere
                }
            }
            Item {
                Layout.alignment: Qt.AlignHCenter
                height: item_height
                width: item_width

                FluIconButton {
                    id: realtimeDataButton

                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.primaryColor.light
                    height: item_icon_button_size
                    iconColor: FluColors.White
                    iconSize: item_icon_size
                    iconSource: FluentIcons.SpeedMedium
                    width: item_icon_button_size

                    onClicked: {
                        showPage(2);
                    }
                }
                FluText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: realtimeDataButton.bottom
                    font.pixelSize: item_font_size
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Realtime Data")
                    width: parent.width
                    wrapMode: Text.WrapAnywhere
                }
            }
            Item {
                Layout.alignment: Qt.AlignHCenter
                height: item_height
                width: item_width

                FluIconButton {
                    id: historyDataButton

                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.primaryColor.light
                    height: item_icon_button_size
                    iconColor: FluColors.White
                    iconSize: item_icon_size
                    iconSource: FluentIcons.History
                    width: item_icon_button_size

                    onClicked: {
                        showPage(3);
                    }
                }
                FluText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: historyDataButton.bottom
                    font.pixelSize: item_font_size
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("History Data")
                    width: parent.width
                    wrapMode: Text.WrapAnywhere
                }
            }
            Item {
                Layout.alignment: Qt.AlignHCenter
                height: item_height
                width: item_width

                FluIconButton {
                    id: errorAnalysisButton

                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.primaryColor.light
                    height: item_icon_button_size
                    iconColor: FluColors.White
                    iconSize: item_icon_size
                    iconSource: FluentIcons.Bug
                    width: item_icon_button_size

                    onClicked: {
                        showPage(4);
                    }
                }
                FluText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: errorAnalysisButton.bottom
                    font.pixelSize: item_font_size
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Error Analysis")
                    width: parent.width
                    wrapMode: Text.WrapAnywhere
                }
            }
            Item {
                Layout.alignment: Qt.AlignHCenter
                height: item_height
                width: item_width

                FluIconButton {
                    id: firmwareUpdateButton

                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.primaryColor.light
                    height: item_icon_button_size
                    iconColor: FluColors.White
                    iconSize: item_icon_size
                    iconSource: FluentIcons.Component
                    width: item_icon_button_size

                    onClicked: {
                        showPage(5);
                    }
                }
                FluText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: firmwareUpdateButton.bottom
                    font.pixelSize: item_font_size
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Firmware Update")
                    width: parent.width
                    wrapMode: Text.WrapAnywhere
                }
            }
            Item {
                Layout.alignment: Qt.AlignHCenter
                height: item_height
                width: item_width

                FluIconButton {
                    id: sysSettingButton

                    anchors.horizontalCenter: parent.horizontalCenter
                    color: FluTheme.primaryColor.light
                    height: item_icon_button_size
                    iconColor: FluColors.White
                    iconSize: item_icon_size
                    iconSource: FluentIcons.Settings
                    width: item_icon_button_size

                    onClicked: {
                        showPage(6);
                    }
                }
                FluText {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: sysSettingButton.bottom
                    font.pixelSize: item_font_size
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("System Setting")
                    width: parent.width
                    wrapMode: Text.WrapAnywhere
                }
            }
        }
    }
    Connections {
        target: BleUart

        onBleConnectChanged: {
            console.log("onBleConnectChanged ", flag);
            hideLoadingDialog();

            if (flag) {
                bluetooothNameLabel.text = qsTr("Bluetooth : ") + ble_name;
                connectButton.iconSource = FluentIcons.DisconnectDrive;
                connectButtonText.text = qsTr("Disconnect");
            } else {
                bluetooothNameLabel.text = qsTr("Bluetooth : ");
                connectButton.iconSource = FluentIcons.MapDrive;
                connectButtonText.text = qsTr("Connect");
            }
        }
        onBleError: {
            bluetoothDialog.close();
            showMessageDialog("BLE Error", info);
        }
        onBleScanError: {
            showMessageDialog("BLE Error", scan_error);
        }
    }
    Connections {
        target: Command

        onActiveNodeListChange: {
            var escList = Command.getNodeActiveList();

            escDevices = [];
            escDeviceDisplay = [];

            escNodeID = ParamManager.getEscID();

            for (var esc_id in escList) {
                var display_name = escList[esc_id];
                escDevices.push(esc_id);
                escDeviceDisplay.push(display_name);
            }

            escComBox.model = escDeviceDisplay;

            comBoxcEscIndex = 0;

            for (var node_id in escList) {
                var esc_id_str = escList[node_id];

                if (esc_id_str === escNodeID.toString()) {
                    escComBox.currentIndex = comBoxcEscIndex;
                    break;
                }

                comBoxcEscIndex++;
            }
        }
    }
}
