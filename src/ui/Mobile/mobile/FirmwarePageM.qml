import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

import FluentUI 1.0

Rectangle {
    id: page

    property int deviceID: 0x20

    Component.onCompleted: {
        console.log("Firmware width: " + width + " height:" + height);
    }
    onVisibleChanged: {
        if (visible) {
            console.log("enter Firmware");
        } else {
            console.log("leave Firmware");
        }
    }

    ColumnLayout {
        id: container

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        clip: true
        spacing: 5
        width: parent.width

        Rectangle {
            id: header

            Layout.fillWidth: true
            color: FluTheme.primaryColor.dark
            implicitHeight: 50

            FluIconButton {
                id: home

                anchors.left: header.left
                anchors.rightMargin: 8
                anchors.top: header.top
                height: 50
                iconColor: FluColors.White
                iconSize: 35
                iconSource: FluentIcons.Back
                width: 50

                onClicked: {
                    showMainPage();
                }
            }
            FluText {
                id: title_label

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: FluColors.White
                font.pixelSize: 24
                text: qsTr("Firmware Update")
            }
        }
        FluArea {
            id: deviceInfoItem

            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.topMargin: 10
            color: "transparent"
            height: 310

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5

                FluText {
                    color: FluTheme.primaryColor.dark
                    font: FluTextStyle.Subtitle
                    text: qsTr("ESC Firmware Update")
                }
                FluText {
                    id: deviceNameLabel

                    text: qsTr("Name:")
                }
                FluText {
                    id: deviceSNCodelabel

                    text: qsTr("SN Code:")
                }
                FluText {
                    id: deviceHardwareIdLabel

                    text: qsTr("Hardware ID:")
                }
                FluText {
                    id: deviceBootloaderVersionLabel

                    text: qsTr("Bootloader Version:")
                }
                FluText {
                    id: deviceSoftVersionLabel

                    text: qsTr("Soft Version:")
                }
                FluText {
                    id: deviceMotorIDLabel

                    text: qsTr("Motor ID:")
                }
                FluText {
                    id: deviceParamVerisonLabel

                    text: qsTr("Param Version:")
                }
                RowLayout {
                    FluText {
                        id: deviceIDLabel

                        text: qsTr("Device ID:")
                    }
                    FluTextBox {
                        id: deviceIDInput

                        Layout.preferredWidth: 80
                        inputMethodHints: Qt.ImhDigitsOnly
                        text: "32"

                        validator: IntValidator {
                            bottom: 1
                            top: 128
                        }
                    }
                }
                RowLayout {
                    FluButton {
                        id: deviceUpdateButton

                        text: qsTr("Firmware Update")
                        width: 150

                        onClicked: {
                            if (!BleUart.isConnected()) {
                                showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Deive First!"));
                                return;
                            }

                            deviceUpdateDialog.dialogTitle = qsTr("ESC Firmware Update");
                            //deviceUpdateDialog.deviceID = parseInt(deviceIDInput.text, 16);
                            deviceUpdateDialog.deviceID = Number(deviceIDInput.text);
                            deviceUpdateDialog.initDialog();
                            deviceUpdateDialog.open();
                        }
                    }
                    FluButton {
                        id: readEscInfoButton

                        font.pixelSize: 14
                        text: qsTr("Read Info")

                        onClicked: {
                            if (!BleUart.isConnected()) {
                                showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                                return;
                            }

                            deviceSNCodelabel.text = qsTr("SN Code:");
                            deviceHardwareIdLabel.text = qsTr("Hardware ID:");
                            deviceBootloaderVersionLabel.text = qsTr("Bootloader Version:");
                            deviceSoftVersionLabel.text = qsTr("Soft Version:");
                            deviceMotorIDLabel.text = qsTr("Motor ID:");
                            deviceParamVerisonLabel.text = qsTr("Param Version:");

                            deviceID = Number(deviceIDInput.text);
                            //deviceID = parseInt(deviceIDInput.text, 16);

                            showLoadingDialog(qsTr("Read Info..."));
                            if (Command.queryESCInfo(deviceID)) {
                                var esc_info = ParamManager.getEscInfoParam();
                                deviceSNCodelabel.text = qsTr("SN Code:") + esc_info.sn_code;
                                deviceHardwareIdLabel.text = qsTr("Hardware ID:") + esc_info.hardware_id;
                                deviceBootloaderVersionLabel.text = qsTr("Bootloader Version:") + esc_info.bootloader_version;
                                deviceSoftVersionLabel.text = qsTr("Soft Version:") + esc_info.app_version;
                                deviceMotorIDLabel.text = qsTr("Motor ID:") + esc_info.motor_id;
                                deviceParamVerisonLabel.text = qsTr("Param Version:") + esc_info.param_version;

                                hideLoadingDialog();
                            } else {
                                hideLoadingDialog();
                                showMessageDialog(qsTr("Error"), qsTr("Read ESC Info Error"));
                            }
                        }
                    }
                    FluButton {
                        id: resetEscConfigButton

                        text: qsTr("Reset ESC Config")

                        onClicked: {
                            if (!BleUart.isConnected()) {
                                showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                                return;
                            }

                            resetEscConfigDialog.open();
                        }
                    }
                    FluContentDialog {
                        id: resetEscConfigDialog

                        buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.PositiveButton
                        message: qsTr("First, Ensuere motor has stopped.\nAre you sure to reset ESC Config?")
                        negativeText: qsTr("No")
                        parent: root
                        positiveText: qsTr("Yes")
                        title: qsTr("Warning")

                        onPositiveClicked: {
                            var result = Command.resetConfig();

                            switch (result) {
                            case -1:
                                showError(qsTr("Reset Esc Config Comm Error"));
                                break;
                            case 0:
                                showSuccess(qsTr("Reset Esc Config Success"));
                                break;
                            case 1:
                                showError(qsTr("Reset Esc Config Failed"));
                                break;
                            case 2:
                                showError(qsTr("Reset Esc Config Not Support"));
                                break;
                            case 5:
                                showError(qsTr("Reset Esc Config Flash Error"));
                                break;
                            case 6:
                                showError(qsTr("Reset Esc Config Need To Stop Motor First"));
                                break;
                            }
                        }
                    }
                }
            }
        }
        FluArea {
            id: se0640dx4Item

            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            color: "transparent"
            height: 200

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5

                FluText {
                    color: FluTheme.primaryColor.dark
                    font: FluTextStyle.Subtitle
                    text: qsTr("Receiver Firmware Update")
                }
                FluText {
                    id: receiverSNCodelabel

                    text: qsTr("SN Code:")
                }
                FluText {
                    id: receiverBootloaderVersionLabel

                    text: qsTr("Bootloader Version:")
                }
                FluText {
                    id: receiveroftVersionLabel

                    text: qsTr("Soft Version:")
                }
                RowLayout {
                    FluButton {
                        id: receiverUpdateButton

                        text: qsTr("Firmware Update")
                        width: 150

                        onClicked: {
                            if (!BleUart.isConnected()) {
                                showMessageDialog(qsTr("Error"), qsTr("Please Connect Ble Device First!"));
                                return;
                            }

                            deviceUpdateDialog.dialogTitle = qsTr("Receiver Firmware Update");
                            deviceUpdateDialog.deviceID = 75;
                            deviceUpdateDialog.initDialog();
                            deviceUpdateDialog.open();
                        }
                    }
                    FluButton {
                        id: readReceiverInfoButton

                        font.pixelSize: 14
                        text: qsTr("Read Info")

                        onClicked: {
                            if (!BleUart.isConnected()) {
                                showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                                return;
                            }

                            receiverSNCodelabel.text = qsTr("SN Code:");
                            receiverBootloaderVersionLabel.text = qsTr("Bootloader Version:");
                            receiveroftVersionLabel.text = qsTr("Soft Version:");

                            showLoadingDialog(qsTr("Read Info..."));
                            if (Command.queryESCInfo(75)) {
                                var esc_info = ParamManager.getEscInfoParam();
                                receiverSNCodelabel.text = qsTr("SN Code:") + esc_info.sn_code;
                                receiverBootloaderVersionLabel.text = qsTr("Bootloader Version:") + esc_info.bootloader_version;
                                receiveroftVersionLabel.text = qsTr("Soft Version:") + esc_info.app_version;

                                hideLoadingDialog();
                            } else {
                                hideLoadingDialog();
                                showMessageDialog(qsTr("Error"), qsTr("Read Receiver Info Error"));
                            }
                        }
                    }
                }
            }
        }
    }
    DeviceUpdateDialogM {
        id: deviceUpdateDialog

        parent: root
    }
    SE0640Dx4UpdateDialogM {
        id: se0640dx4UpdateDialog

        parent: root
    }
}
