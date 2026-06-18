import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

import FluentUI 1.0

FluPopup {
    id: popup

    property int deviceID: 0x20
    property alias dialogTitle: text_title.text

    function initDialog() {
        updateBar.progress = 0;
        infoTextArea.text = "";
    }

    focus: true
    implicitHeight: 600
    implicitWidth: 600

    Rectangle {
        id: layout_content

        anchors.fill: parent
        color: 'transparent'
        radius: 5

        FluText {
            id: text_title

            font: FluTextStyle.Subtitle
            leftPadding: 20
            rightPadding: 20
            text: title
            topPadding: 20
            wrapMode: Text.WrapAnywhere

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
        }
        Rectangle {
            id: contentRect

            anchors {
                bottom: layout_actions.top
                left: parent.left
                right: parent.right
                top: text_title.bottom
            }
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                FluText {
                    elide: Label.ElideLeft
                    text: qsTr("Device ID : ") + deviceID.toString()
                }
                FluButton {
                    id: chooseFileButton

                    text: qsTr("Choose File")

                    onClicked: {
                        if (Utility.requestFilePermission()) {
                            selectFileDialog.open();
                        } else {
                            showMessageDialog(qsTr("Error"), qsTr("Unable to access file system."));
                        }
                    }
                }
                FluText {
                    elide: Label.ElideLeft
                    text: qsTr("File Path:")
                }
                FluText {
                    id: filePathLabel

                    Layout.fillWidth: true
                    elide: Label.ElideLeft
                    text: ""
                    wrapMode: Text.WrapAnywhere
                }
                FluButton {
                    id: updateButton

                    text: qsTr("Start Update")

                    onClicked: {
                        if (filePathLabel.text === "") {
                            showMessageDialog(qsTr("Error"), qsTr("Please Select File First!"));
                            return;
                        }

                        chooseFileButton.enabled = false;
                        closeButton.enabled = false;
                        updateButton.enabled = false;
                        updateButton.text = qsTr("Firmware Updating");

                        infoTextArea.text = "";
                        FirmwareUpdate.updateFile(filePathLabel.text, deviceID);

                        chooseFileButton.enabled = true;

                        closeButton.enabled = true;
                        updateButton.enabled = true;

                        updateButton.text = qsTr("Firmware Update");
                    }
                }
                FluText {
                    elide: Label.ElideLeft
                    text: qsTr("Update Process:")
                }
                FluProgressBar {
                    id: updateBar

                    Layout.fillWidth: true
                    indeterminate: false
                    progress: 0
                }
                FluText {
                    id: infoLabel

                    elide: Label.ElideLeft
                    text: qsTr("Update Info")
                }
                ScrollView {
                    id: messageTextAreaScrollView

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    background: Rectangle {
                        anchors.fill: parent
                        border.color: infoTextArea.enabled ? Qt.rgba(FluTheme.primaryColor.dark.r, FluTheme.primaryColor.dark.g, FluTheme.primaryColor.dark.b, 0.3) : "transparent"
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
        Rectangle {
            id: layout_actions

            color: FluTheme.dark ? Qt.rgba(32 / 255, 32 / 255, 32 / 255, blurBackground ? blurOpacity - 0.4 : 1) : Qt.rgba(243 / 255, 243 / 255, 243 / 255, blurBackground ? blurOpacity - 0.4 : 1)
            height: 68
            radius: 5

            anchors {
                bottom: layout_content.bottom
                left: parent.left
                right: parent.right
            }
            RowLayout {
                spacing: 15

                anchors {
                    centerIn: parent
                    fill: parent
                    margins: spacing
                }
                Item {
                    Layout.fillWidth: true
                }
                FluFilledButton {
                    id: closeButton

                    Layout.fillHeight: true
                    text: "Close"

                    onClicked: {
                        popup.close();
                    }
                }
            }
        }
    }
    FileBrowerM {
        id: selectFileDialog

        anchors.fill: parent
        folder: Config.getFirmwarePath()
        nameFilters: "*"

        onFileSelected: {
            console.log(folder);
            console.log(file);

            Config.saveFirmwarePath(folder);
            filePathLabel.text = file;
        }
    }
    Connections {
        target: FirmwareUpdate

        onUpdateMessage: {
            infoTextArea.append(message);
        }
        onUpdateProcess: {
            updateBar.progress = progeress / 100;
        }
    }
}
