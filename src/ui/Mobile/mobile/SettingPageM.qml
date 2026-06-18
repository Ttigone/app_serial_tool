import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

import FluentUI 1.0

Item{
    id:settingPage

    Component.onCompleted: {
    }

    onVisibleChanged: {
        if(visible)
        {
            console.log("enter SysSetting")

            initPage();
        }
        else
        {
            console.log("leave SysSetting")

        }
    }

    function initPage(){
        //语言
        if(Config.getLanguage() === "zh_cn")
        {
            languageComBox.currentIndex = 0;
        }
        else
        {
            languageComBox.currentIndex = 1;
        }

        //电调扫描
        escStartIDInput.text = Config.getEscScanStartID().toString();
        escEndIDInput.text = Config.getEscScanEndID().toString();
    }

    ColumnLayout{
        id:container
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.left: parent.left
        anchors.right: parent.right
        width: parent.width
        spacing: 5
        clip: true

        Rectangle{
            id:header
            color: FluTheme.primaryColor.dark
            Layout.fillWidth: true
            implicitHeight : 50

            FluIconButton {
                id: home
                anchors.left: header.left
                anchors.top: header.top
                anchors.rightMargin: 8
                width: 50
                height: 50
                iconSize:35
                iconSource:FluentIcons.Back
                iconColor: FluColors.White
                onClicked: {
                    showMainPage();
                }
            }

            FluText {
                id:title_label
                text: qsTr("System Setting")
                font.pixelSize: 24
                color: FluColors.White
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter : parent.verticalCenter
            }
        }

        FluArea{
            implicitWidth: settingPage.width - 20
            implicitHeight: 120

            Layout.leftMargin: 10

            ColumnLayout{
                anchors.fill: parent
                anchors.margins:10

                FluText{
                    text: qsTr("Language")

                    font : FluTextStyle.Caption
                    color: FluTheme.primaryColor.dark
                }

                RowLayout{
                    spacing: 5
                    FluText{
                        text: qsTr("Language:")
                    }
                    FluComboBox {
                        id:languageComBox
                        Layout.preferredWidth: 250
                        model: ["中文", "English"]
                        currentIndex: 0
                        onCurrentIndexChanged:
                        {

                        }
                    }
                }

                FluButton{
                    id:langugeApplyButton
                    text: qsTr("Apply")
                    Layout.preferredWidth: 100
                    onClicked:
                    {
                        if(languageComBox.currentIndex == 0)
                        {
                            Config.saveLanguage("zh_cn");
                        }
                        else
                        {
                            Config.saveLanguage("en_us");
                        }
                        showMessageDialog(qsTr("Warning"), qsTr("Restart Application to Make Language in Effect!"));
                    }
                }
            }
        }

        FluArea{
            implicitWidth: settingPage.width - 20
            implicitHeight: 160
            Layout.leftMargin: 10

            ColumnLayout{
                anchors.fill: parent
                anchors.margins:10

                FluText{
                    text: qsTr("ESC Scan")

                    font : FluTextStyle.Caption
                    color: FluTheme.primaryColor.dark
                }

                RowLayout{
                    spacing: 5
                    FluText{
                        text: qsTr("ESC Start ID:")
                        horizontalAlignment:Text.AlignHCenter
                        Layout.preferredWidth: 80
                    }

                    FluTextBox {
                        id: escStartIDInput
                        implicitWidth: 100
                        inputMethodHints:Qt.ImhDigitsOnly

                        text:"32"
                        validator: IntValidator{bottom: 1; top: 128;}
                        horizontalAlignment:Text.AlignHCenter
                    }
                }

                RowLayout{
                    spacing: 5
                    FluText{
                        text: qsTr("ESC End ID:")
                        horizontalAlignment:Text.AlignHCenter
                        Layout.preferredWidth: 80
                    }

                    FluTextBox {
                        id: escEndIDInput
                        implicitWidth: 100
                        inputMethodHints:Qt.ImhDigitsOnly

                        text:"35"
                        validator: IntValidator{bottom: 1; top: 128;}
                        horizontalAlignment:Text.AlignHCenter
                    }
                }

                FluButton{
                    id:scanEscApplyButton
                    text: qsTr("Apply")
                    Layout.preferredWidth: 100
                    onClicked:
                    {
                        var start_id = Number(escStartIDInput.text);
                        var end_id = Number(escEndIDInput.text);
                        Config.saveEscScan(start_id, end_id);

                        showSuccess(qsTr("Apply ESC Scan Setting Success!"));
                    }
                }
            }
        }

        Item{
            Layout.fillHeight: true
        }

        FluText{
            Layout.margins: 10
            text:"Sinemo " + " (Build Time:" + Config.getBuildDateTime() + ")"
        }
    }
}
