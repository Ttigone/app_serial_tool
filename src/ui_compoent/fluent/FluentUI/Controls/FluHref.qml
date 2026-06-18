import QtQuick 2.12
import QtQuick.Controls 2.12
import FluentUI 1.0

FluText {
    signal hrefClicked()
    property string href
    property alias hovered: mouseArea.containsMouse
    property string toolTip: ""

    //color: hovered ? Qt.darker(CusConfig.primaryColor, 1.2) : CusConfig.primaryColor

    color: hovered ? FluTheme.primaryColor.darker : FluTheme.primaryColor.lighter

    ToolTip.visible:((toolTip != "") && hovered)
    ToolTip.delay: 200
    ToolTip.text: toolTip

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: {
            if(href != null){
                Qt.openUrlExternally(href)
            }
            hrefClicked()
        }
    }
}
