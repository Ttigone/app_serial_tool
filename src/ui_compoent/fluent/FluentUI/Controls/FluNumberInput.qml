import QtQuick 2.12
import QtQuick.Controls 2.12
import FluentUI 1.0

TextField {
    id: control
    placeholderText: ""
    font.pixelSize: 12
    width: 150
    height: 30
    selectByMouse: true

    background: Rectangle {
        implicitWidth: parent.width
        implicitHeight: parent.height
        Rectangle{
            color: control.activeFocus ? FluTheme.primaryColor.dark : "#b7b7b7"
            width: parent.width
            height: 2
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            Behavior on color { ColorAnimation {duration: 100} }
        }
    }
}
