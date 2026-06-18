import QtQuick 2.12
import QtQuick.Controls 2.12
import FluentUI 1.0

Text {
    property int iconSource
    property int iconSize: 20
    property color iconColor: FluTheme.dark ? "#FFFFFF" : "#000000"
    property string iconFontFamily: "Segoe Fluent Icons"
    id:text_icon
    font.family: iconFontFamily
    font.pixelSize: iconSize
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    color: iconColor
    text: (String.fromCharCode(iconSource).toString(16))
}
