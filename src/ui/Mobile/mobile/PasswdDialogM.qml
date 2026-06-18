import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

import FluentUI 1.0
import QmlCustomEditPlot 1.0


FluPopup {
    id: popup
    property string title: "Password"

    signal cancelClicked
    signal applyClicked(string passwd)

    property var minWidth: {
        if(Window.window==null)
            return 400
        return  Math.min(Window.window.width,400)
    }
    focus: true

    Rectangle {
        id:layout_content
        anchors.fill: parent
        implicitWidth:minWidth
        implicitHeight: text_title.height + passwdItem.height + layout_actions.height
        color: 'transparent'
        radius:5
        FluText{
            id:text_title
            font: FluTextStyle.Subtitle
            text:title
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

        Item
        {
            id:passwdItem
            height: 60

            anchors{
                top:text_title.bottom
                left: parent.left
                right: parent.right
            }

            RowLayout{
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10
                FluText{
                    Layout.preferredWidth: 80
                    text: qsTr("Password:")
                }

                FluTextBox{
                    id: passwdTextInput
                    maximumLength: 10
                    Layout.fillWidth: true
                }
            }
        }

        Rectangle{
            id:layout_actions
            height: 68
            radius: 5
            color: FluTheme.dark ? Qt.rgba(32/255,32/255,32/255, blurBackground ? blurOpacity - 0.4 : 1) : Qt.rgba(243/255,243/255,243/255,blurBackground ? blurOpacity - 0.4 : 1)
            anchors{
                top:passwdItem.bottom
                left: parent.left
                right: parent.right
            }
            RowLayout{
                anchors
                {
                    centerIn: parent
                    margins: spacing
                    fill: parent
                }
                spacing: 15

                FluButton{
                    id:cancel_btn
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: qsTr("Cancel")
                    onClicked: {
                        popup.close()
                        cancelClicked()
                    }
                }
                FluFilledButton{
                    id:apply_btn
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: qsTr("Apply")
                    onClicked: {
                        popup.close();
                        applyClicked(passwdTextInput.text);
                    }
                }
            }
        }
    }
}
