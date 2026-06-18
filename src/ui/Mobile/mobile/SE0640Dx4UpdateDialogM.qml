import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import QtQuick.Dialogs 1.3

import FluentUI 1.0

FluPopup {
    id:popup
    implicitWidth: 600
    implicitHeight: 600

    property int deviceID: 0x20
    property alias dialogTitle: text_title.text

    property int updatingDeviceID: 0x20


    function initDialog(){
        m1UpdateBar.progress = 0;
        m2UpdateBar.progress = 0;
        m3UpdateBar.progress = 0;
        m4UpdateBar.progress = 0;

        infoTextArea.text = "";
    }

    focus: true

    Rectangle {
        id:layout_content
        anchors.fill: parent
        color: 'transparent'
        radius:5
        FluText{
            id:text_title
            font: FluTextStyle.Subtitle
            text:qsTr("SE0640Dx4 ESC Firmware Update")
            topPadding: 20
            leftPadding: 20
            rightPadding: 20
            wrapMode: Text.WrapAnywhere
            anchors{
                top:parent.top
                left: parent.left
                right: parent.right
            }
        }

        Rectangle{
            id: contentRect

            anchors{
                top: text_title.bottom
                left: parent.left
                right: parent.right
                bottom: layout_actions.top
            }

            ColumnLayout{
                anchors.margins: 10
                anchors.fill: parent
                spacing: 5

                Row {
                    id:escCheckGorup
                    FluCheckBox {
                        id:m1ChcekBox
                        checked: true
                        text: qsTr("M1(32)")
                    }
                    FluCheckBox {
                        id:m2ChcekBox
                        checked: true
                        text: qsTr("M2(33)")
                    }
                    FluCheckBox {
                        id:m3ChcekBox
                        checked: true
                        text: qsTr("M3(34)")
                    }
                    FluCheckBox {
                        id:m4ChcekBox
                        checked: true
                        text: qsTr("M4(35)")
                    }
                }

                FluButton {
                    id:chooseFileButton
                    text: qsTr("Choose File")
                    onClicked: {
                        if(Utility.requestFilePermission())
                        {
                            selectFileDialog.open();
                        }
                        else
                        {
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
                    Layout.fillWidth:true
                    elide: Label.ElideLeft
                    wrapMode: Text.WrapAnywhere
                    text: ""
                }

                FluButton {
                    id: updateButton
                    text: qsTr("Start Update")
                    onClicked: {
                        if(filePathLabel.text === "")
                        {
                            showMessageDialog(qsTr("Error"), qsTr("Please Select File First!"));
                            return;
                        }

                        escCheckGorup.enabled = false;
                        chooseFileButton.enabled = false;
                        closeButton.enabled = false;
                        updateButton.enabled = false;
                        updateButton.text = qsTr("Firmware Updating")

                        infoTextArea.text = "";
                        m1UpdateBar.progress = 0;
                        m2UpdateBar.progress = 0;
                        m3UpdateBar.progress = 0;
                        m4UpdateBar.progress = 0;

                        if(m1ChcekBox.checked)
                        {
                            updatingDeviceID = 0x20;
                            infoTextArea.append("M1 Firmware Updating");
                            FirmwareUpdate.updateFile(filePathLabel.text, updatingDeviceID);
                        }

                        if(m2ChcekBox.checked)
                        {
                            updatingDeviceID = 0x21;
                            infoTextArea.append("M2 Firmware Updating");
                            FirmwareUpdate.updateFile(filePathLabel.text, updatingDeviceID);
                        }

                        if(m3ChcekBox.checked)
                        {
                            updatingDeviceID = 0x22;
                            infoTextArea.append("M3 Firmware Updating");
                            FirmwareUpdate.updateFile(filePathLabel.text, updatingDeviceID);
                        }

                        if(m4ChcekBox.checked)
                        {
                            updatingDeviceID = 0x23;
                            infoTextArea.append("M4 Firmware Updating");
                            FirmwareUpdate.updateFile(filePathLabel.text, updatingDeviceID);
                        }

                        escCheckGorup.enabled = true;
                        chooseFileButton.enabled = true;
                        closeButton.enabled = true;
                        updateButton.enabled = true;

                        updateButton.text = qsTr("Firmware Update")
                    }
                }


                FluText {
                    elide: Label.ElideLeft
                    text: qsTr("Update Process:")
                }

                RowLayout {
                    FluText {
                        text: qsTr("M1(32): ")
                    }

                    FluProgressBar{
                        id:m1UpdateBar
                        progress:0
                        indeterminate: false
                        Layout.fillWidth: true
                    }
                }


                RowLayout {
                    FluText {
                        text: qsTr("M2(33): ")
                    }

                    FluProgressBar{
                        id:m2UpdateBar
                        progress:0
                        indeterminate: false
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    FluText {
                        text: qsTr("M3(34): ")
                    }

                    FluProgressBar{
                        id:m3UpdateBar
                        progress:0
                        indeterminate: false
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    FluText {
                        text: qsTr("M4(35): ")
                    }

                    FluProgressBar{
                        id:m4UpdateBar
                        progress:0
                        indeterminate: false
                        Layout.fillWidth: true
                    }
                }

                FluText {
                    id:infoLabel
                    elide: Label.ElideLeft
                    text: qsTr("Update Info")
                }

                ScrollView {
                    id: messageTextAreaScrollView
                    Layout.fillWidth:true
                    Layout.fillHeight:true

                    background:  Rectangle {
                        anchors.fill: parent
                        border.color: infoTextArea.enabled ? Qt.rgba(FluTheme.primaryColor.dark.r, FluTheme.primaryColor.dark.g, FluTheme.primaryColor.dark.b, 0.3) : "transparent"
                    }

                    TextArea {
                        id: infoTextArea
                        selectByMouse: true
                        antialiasing: true
                        readOnly: true
                        font.pixelSize: 12
                        textFormat: Text.RichText
                        text: ""

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.RightButton
                            cursorShape: Qt.IBeamCursor
                            onClicked: {
                                if (mouse.button === Qt.RightButton)
                                    var start = infoTextArea.selectionStart
                                var end = infoTextArea.selectionEnd
                                messageContextMenu.popup()
                                infoTextArea.select(start,end)
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

        Rectangle{
            id:layout_actions
            height: 68
            radius: 5
            color: FluTheme.dark ? Qt.rgba(32/255,32/255,32/255, blurBackground ? blurOpacity - 0.4 : 1) : Qt.rgba(243/255,243/255,243/255,blurBackground ? blurOpacity - 0.4 : 1)
            anchors{
                left: parent.left
                right: parent.right
                bottom:layout_content.bottom
            }
            RowLayout{
                anchors
                {
                    centerIn: parent
                    margins: spacing
                    fill: parent
                }
                spacing: 15
                Item{
                    Layout.fillWidth: true
                }

                FluFilledButton{
                    id:closeButton

                    Layout.fillHeight: true
                    text: "Close"
                    onClicked: {
                        popup.close()
                    }
                }
            }
        }
    }

    FileBrowerM {
        id: selectFileDialog
        anchors.fill: parent
        folder: Config.getFirmwarePath()
        nameFilters:"*"
        onFileSelected:{
            console.log(folder)
            console.log(file)

            Config.saveFirmwarePath(folder);
            filePathLabel.text = file
        }
    }

    Connections {
        target: FirmwareUpdate
        onUpdateProcess: {

            if(updatingDeviceID == 0x20)
            {
                m1UpdateBar.progress = progeress/100;
            }
            if(updatingDeviceID == 0x21)
            {
                m2UpdateBar.progress = progeress/100;
            }
            if(updatingDeviceID == 0x22)
            {
                m3UpdateBar.progress = progeress/100;
            }
            if(updatingDeviceID == 0x23)
            {
                m4UpdateBar.progress = progeress/100;
            }
        }

        onUpdateMessage: {
            infoTextArea.append(message);
        }
    }
}
