import QtQuick 2.10
import QtQuick.Controls 2.3

import FluentUI 1.0

ListView {
    id: listView
    spacing: 0
    clip: true

    property color primaryColor :FluTheme.primaryColor.dark

    property string viewBgColor: Qt.rgba(primaryColor.r, primaryColor.g, primaryColor.b, 0.08)
    property string itemBgColor: Qt.rgba(primaryColor.r, primaryColor.g, primaryColor.b, 0.2)
    property string itemHoverBgColor: Qt.rgba(primaryColor.r, primaryColor.g, primaryColor.b, 0.3)
    property string itemSelectedBgColor: Qt.rgba(primaryColor.r, primaryColor.g, primaryColor.b, 0.4)
    property string itemPressedBgColor: Qt.rgba(primaryColor.r, primaryColor.g, primaryColor.b, 0.5)

    property var itemModel: []

    property var clickedCall: null

    currentIndex: 0

    onItemModelChanged: {
        drawModel()
    }

    Rectangle{
        anchors.fill: parent
        color: parent.viewBgColor
        z: -1
    }

    function drawModel(){
        listModel.clear()
        if(itemModel.length > 0){
            for(var i = 0; i < itemModel.length; i++){
                listModel.append(itemModel[i])
            }
        }
        currentIndex = currentIndex
    }

    property ListModel listModel: ListModel {}


    delegate: ItemDelegate {
        id: itemDelegate
        width: listView.width
        height: itemVisible ? 40 :0
        highlighted: ListView.isCurrentItem
        visible: itemVisible
        Item {
            anchors.fill: parent
            property bool fontIconExist: typeof(fontIcon) !== "undefined"

            Rectangle {
                id:placeholder
                anchors{
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: 10
                color: "transparent"
            }

            FluIcon{
                id: fontIconText
                font.pixelSize: 20
                iconSource: parent.fontIconExist ? fontIcon : ""
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: placeholder.right
                width: parent.fontIconExist ? 20 : 0
                color: primaryColor
            }

            FluText {
                text: name
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: fontIconText.right
                anchors.leftMargin: 10
            }
        }

        onClicked: {
            listView.currentIndex = index
            if(clickedCall != null){
                clickedCall(index,listModel[index])
            }
        }

        background: Rectangle {
            color: highlighted ? listView.itemSelectedBgColor :
                                 (parent.hovered ? (parent.pressed ? listView.itemPressedBgColor : listView.itemHoverBgColor) : listView.itemBgColor)
            Rectangle {
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                height: parent.height
                width: 4
                color: highlighted ? primaryColor : "transparent"
            }
        }
    }
    model: listView.listModel
}
