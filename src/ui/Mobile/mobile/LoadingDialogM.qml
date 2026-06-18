import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

import FluentUI 1.0

FluPopup {
    id: popup

    property string loadingText: "Loading"

    focus: true

    Rectangle {
        id: layout_content

        anchors.fill: parent
        color: FluTheme.dark ? Qt.rgba(1, 1, 1, 0.03) : Qt.rgba(0, 0, 0, 0.03)
        radius: 5

        ColumnLayout {
            anchors.centerIn: parent

            FluProgressRing {
                Layout.alignment: Qt.AlignHCenter
                indeterminate: true
            }
            FluText {
                Layout.alignment: Qt.AlignHCenter
                font: FluTextStyle.Subtitle
                text: loadingText
            }
        }
    }
}
