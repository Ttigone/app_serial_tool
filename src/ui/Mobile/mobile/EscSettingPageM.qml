import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import Qt.labs.qmlmodels 1.0

import FluentUI 1.0

Item {
    id: page

    property int curTab: 0

    onVisibleChanged: {
        if (visible) {
            console.log("enter EscSetting");
        } else {
            console.log("leave EscSetting");
        }
    }

    //    SwipeArea {
    //        id: swipeArea
    //        anchors.fill: parent

    //        onSwipeLeft: {
    //            showMainPage();
    //        }
    //    }

    ColumnLayout {
        id: container

        anchors.bottom: parent.bottom
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
                text: qsTr("ESC Setting")
            }
        }
        RowLayout {
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            FluButton {
                id: unlockConfigButton

                Layout.fillWidth: true
                text: qsTr("Unlock")

                onClicked: {
                    if (!BleUart.isConnected()) {
                        showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                        return;
                    }
                    unlockPasswdDialog.open();
                }
            }
            PasswdDialogM {
                id: unlockPasswdDialog

                parent: root
                title: qsTr("Unclock Password")

                onApplyClicked: {
                    console.log(passwd);
                    var result = Command.unlockConfig(passwd);

                    switch (result) {
                    case -1:
                        showError(qsTr("Unlock Config Comm Error"));
                        break;
                    case 0:
                        showSuccess(qsTr("Unlock Config Success"));
                        break;
                    case 1:
                        showError(qsTr("Unlock Config Failed"));
                        break;
                    case 2:
                        showError(qsTr("Unlock Config Not Support"));
                        break;
                    case 4:
                        showError(qsTr("Unlock Config Data Size Error"));
                        break;
                    default:
                        showError(qsTr("Unlock Config Failed"));
                        break;
                    }
                }
            }
            FluButton {
                id: exportFileButton

                Layout.fillWidth: true
                text: qsTr("Export File")

                onClicked: {
                    var file_url = Config.getEscConfigPath() + "/" + Qt.formatDateTime(new Date(), "yyyy-MM-dd hh-mm-ss") + ".json";

                    console.log(file_url);

                    if (Config.saveEscConfig(file_url)) {
                        showSuccess("Export Config File " + file_url + " Success..");
                    } else {
                        showError("Export Config File " + file_url + " Error..");
                    }
                }
            }
        }
        RowLayout {
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            FluButton {
                id: readConfigButton

                Layout.fillWidth: true
                text: qsTr("Read Config")

                onClicked: {
                    if (!BleUart.isConnected()) {
                        showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                        return;
                    }

                    showLoadingDialog(qsTr("Read Config..."));

                    var result = Command.getAllParam();

                    hideLoadingDialog();

                    if (result === "") {
                        showSuccess(qsTr("Read All Param Success"));
                    } else {
                        showMessageDialog(qsTr("Read Config"), result);
                    }
                }
            }
            FluButton {
                id: writeConfigButton

                Layout.fillWidth: true
                text: qsTr("Write Config")
                visible: false

                onClicked: {
                    if (!BleUart.isConnected()) {
                        showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                        return;
                    }

                    writeConfigDialog.open();
                }
            }
            FluContentDialog {
                id: writeConfigDialog

                buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.PositiveButton
                message: qsTr("First, Ensuere motor has stopped.\nAre you sure to write all param config?")
                negativeText: qsTr("No")
                parent: root
                positiveText: qsTr("Yes")
                title: qsTr("Warning")

                onPositiveClicked: {
                    showLoadingDialog(qsTr("Write Config..."));

                    var result = Command.setAllParam();

                    hideLoadingDialog();

                    if (result === "") {
                        showSuccess(qsTr("Write All Param Success"));
                    } else {
                        showMessageDialog(qsTr("Write Config"), result);
                    }
                }
            }
            FluButton {
                id: saveConfigButton

                Layout.fillWidth: true
                text: qsTr("Save Config")

                onClicked: {
                    if (!BleUart.isConnected()) {
                        showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                        return;
                    }

                    saveConfigDialog.open();
                }
            }
            FluContentDialog {
                id: saveConfigDialog

                buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.PositiveButton
                message: qsTr("First, Ensuere motor has stopped.\nAre you sure to save config?")
                negativeText: qsTr("No")
                parent: root
                positiveText: qsTr("Yes")
                title: qsTr("Warning")

                onPositiveClicked: {
                    showLoadingDialog(qsTr("Save Config..."));

                    var result = Command.writeFlash();

                    hideLoadingDialog();

                    switch (result) {
                    case -1:
                        showError(qsTr("Save Config Comm Error"));
                        break;
                    case 0:
                        showSuccess(qsTr("Save Config Success"));
                        break;
                    case 1:
                        showError(qsTr("ESC Flash Error"));
                        break;
                    case 2:
                        showError(qsTr("ESC Need Stop Motor First"));
                        break;
                    }
                }
            }
            FluButton {
                id: restartEscButton

                Layout.fillWidth: true
                text: qsTr("Restart ESC")

                onClicked: {
                    if (!BleUart.isConnected()) {
                        showMessageDialog(qsTr("Error"), qsTr("Please Connect Serial Device First!"));
                        return;
                    }

                    restartEscDialog.open();
                }
            }
            FluContentDialog {
                id: restartEscDialog

                buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.PositiveButton
                message: qsTr("First, Ensuere motor has stopped.\nAre you sure to restart ESC?")
                negativeText: qsTr("No")
                parent: root
                positiveText: qsTr("Yes")
                title: qsTr("Warning")

                onPositiveClicked: {
                    var result = FirmwareUpdate.restartESC();

                    switch (result) {
                    case -1:
                        showError(qsTr("Restart Esc Comm Error"));
                        break;
                    case 0:
                        showSuccess(qsTr("Restart Esc Success"));
                        break;
                    case 1:
                        showError(qsTr("Restart Esc Failure, Please Stop motor First"));
                        break;
                    }
                }
            }
        }
        Item {
            id: tabItem

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10

            FluTabMenu {
                id: tabMenu

                clickedCall: function (index) {
                    curTab = index;
                }
                height: 30
                itemModel: [
                    {
                        name: qsTr("Board")
                    },
                    {
                        name: qsTr("Motor")
                    },
                    {
                        name: qsTr("Control")
                    },
                    {
                        name: qsTr("Speed")
                    },
                    {
                        name: qsTr("Position")
                    },
                    {
                        name: qsTr("Throttle")
                    },
                    {
                        name: qsTr("Advance")
                    },
                    {
                        name: qsTr("Observer")
                    },
                    {
                        name: qsTr("Protect")
                    },
                ]

                anchors {
                    left: tabItem.left
                    right: tabItem.right
                    top: tabItem.top
                }
            }
            StackLayout {
                anchors.bottomMargin: 10
                anchors.topMargin: 10
                currentIndex: curTab

                anchors {
                    bottom: tabItem.bottom
                    left: tabItem.left
                    right: tabItem.right
                    top: tabMenu.bottom
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Board Param")
                        groupName: "board"
                    }
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Motor Param")
                        groupName: "motor"
                    }
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Control Param")
                        groupName: "control"
                    }
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Speed Param")
                        groupName: "speed"
                    }
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Position Param")
                        groupName: "position"
                    }
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Throttle Param")
                        groupName: "throttle"
                    }
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Advance Param")
                        groupName: "advance"
                    }
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Observer Param")
                        groupName: "observer"
                    }
                }
                FluScrollablePage {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ParamTableM {
                        Layout.fillWidth: true
                        groupDisplayName: qsTr("Protect Param")
                        groupName: "protect"
                    }
                }
            }
        }
    }
}
