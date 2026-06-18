import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.0

import FluentUI 1.0

Item {
    property int testingDeviceID: 32

    function refreshErrorInfo() {
        infoTextArea.text = "";

        infoTextArea.append(qsTr("<font color=#0066B4> ############### Running State ############### </font>"));
        var running_error = Number(runnigErrorTextBox.text);
        if (running_error == 0) {
            infoTextArea.append("No Error\n");
        } else {
            var running_error_info = ParamManager.getRunningErrorInfo(running_error);
            infoTextArea.append(running_error_info);
        }

        infoTextArea.append(qsTr("<font color=#0066B4>############### Selfcheck Error ############### </font>"));
        var self_error = Number(selfCheckErrorTextBox.text);
        if (self_error == 0) {
            infoTextArea.append("No Error\n");
        } else {
            var self_info = ParamManager.getSelfCheckErrorInfo(self_error);
            infoTextArea.append(self_info);
        }
    }

    anchors.margins: 10

    onVisibleChanged: {
        if (visible) {
            console.log("enter Analysis");

            refreshErrorInfo();
        } else {
            console.log("leave Analysis");
        }
    }

    ColumnLayout {
        id: container

        clip: true
        spacing: 5
        width: parent.width

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
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
                text: qsTr("Error Analysis")
            }
        }
        FluArea {
            id: selfCheckErrorItem

            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.topMargin: 10
            color: "transparent"
            height: 450

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5

                FluText {
                    color: FluTheme.primaryColor.dark
                    font: FluTextStyle.Subtitle
                    text: qsTr("ESC Error Analysis")
                }
                FluButton {
                    id: readEscInfoButton

                    text: qsTr("Read Error")

                    onClicked: {
                        if (!BleUart.isConnected()) {
                            showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                            return;
                        }

                        testingDeviceID = ParamManager.getEscID();

                        if (Command.queryRealtimeData(testingDeviceID)) {
                            refreshErrorInfo();
                        } else {
                            showMessageDialog(qsTr("Error"), qsTr("Read ESC Info Error"));
                        }
                    }
                }
                RowLayout {
                    FluText {
                        id: runningErrorLabel

                        Layout.preferredWidth: 120
                        horizontalAlignment: Text.AlignHCenter
                        text: qsTr("Running State: ")
                    }
                    FluTextBox {
                        id: runnigErrorTextBox

                        Layout.preferredWidth: 100
                        inputMethodHints: Qt.ImhDigitsOnly
                        text: qsTr("0")

                        validator: IntValidator {
                            bottom: 0
                            top: 65535
                        }

                        onCommit: {
                            refreshErrorInfo();
                        }
                    }
                }
                RowLayout {
                    FluText {
                        id: selfCheckErrorLabel

                        Layout.preferredWidth: 120
                        horizontalAlignment: Text.AlignHCenter
                        text: qsTr("Selfcheck Error: ")
                    }
                    FluTextBox {
                        id: selfCheckErrorTextBox

                        Layout.preferredWidth: 100
                        inputMethodHints: Qt.ImhDigitsOnly
                        text: qsTr("0")

                        validator: IntValidator {
                            bottom: 0
                            top: 65535
                        }

                        onCommit: {
                            refreshErrorInfo();
                        }
                    }
                }
                FluText {
                    text: qsTr("Error Info :")
                }
                ScrollView {
                    id: messageTextAreaScrollView

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    background: FluRectangle {
                        anchors.fill: parent
                    }

                    TextArea {
                        id: infoTextArea

                        antialiasing: true
                        font.pixelSize: 14
                        readOnly: true
                        selectByMouse: true
                        text: ""
                        textFormat: Text.RichText

                        MouseArea {
                            acceptedButtons: Qt.RightButton
                            anchors.fill: parent
                            cursorShape: Qt.IBeamCursor

                            onClicked: {
                                if (mouse.button === Qt.RightButton)
                                    var start = infoTextArea.selectionStart;
                                var end = infoTextArea.selectionEnd;
                                messageContextMenu.popup();
                                infoTextArea.select(start, end);
                            }

                            Menu {
                                id: messageContextMenu

                                MenuItem {
                                    text: qsTr("Copy")

                                    onTriggered: infoTextArea.copy()
                                }
                            }
                        }
                    }
                }
            }
        }
        FluArea {
            id: historyRunningErrorItem

            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.topMargin: 10
            color: "transparent"
            height: 200

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5

                FluText {
                    color: FluTheme.primaryColor.dark
                    font: FluTextStyle.Subtitle
                    text: qsTr("History Running State")
                }
                RowLayout {
                    FluText {
                        id: readErrorIndexLabel

                        //                        horizontalAlignment:Text.AlignHCenter
                        Layout.preferredWidth: 100
                        text: qsTr("Start Index: ")
                    }
                    FluTextBox {
                        id: historyErrorIndexBox

                        Layout.preferredWidth: 100
                        text: qsTr("0")

                        validator: IntValidator {
                            bottom: 0
                            top: 512
                        }
                    }
                }
                FluText {
                    //                    horizontalAlignment:Text.AlignHCenter
                    Layout.preferredWidth: 150
                    text: qsTr("History State Code: ")
                }
                FluTextBox {
                    id: historyErrorCodeBox

                    Layout.preferredWidth: 300
                    enabled: false
                    text: "    0     0     0     0"
                }
                FluButton {
                    id: readHistoryErrorButton

                    text: qsTr("Read Error")

                    onClicked: {
                        if (!BleUart.isConnected()) {
                            showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                            return;
                        }

                        var start_index = Number(historyErrorIndexBox.text);
                        if (Command.queryHistoryErrorCode(start_index, 4)) {
                            var esc_info = ParamManager.getEscInfoParam();
                            historyErrorCodeBox.text = esc_info.history_error_code;
                        } else {
                            showMessageDialog(qsTr("Error"), qsTr("Read History State Code Error"));
                        }
                    }
                }
            }
        }
    }
    Connections {
        target: Command

        onRealtimeDataReceived: {
            selfCheckErrorTextBox.text = realtime_data.selfcheck_error;
            runnigErrorTextBox.text = realtime_data.running_error;
        }
    }
}
