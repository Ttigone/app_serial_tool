import QtQuick 2.12
import QtQuick.Controls 2.12
import FluentUI 1.0

ListView {
    id: listView
    orientation: ListView.Horizontal
    height: 35
    spacing: 0
    clip: true
    currentIndex: 0
    property string highlightedColor: FluTheme.primaryColor.dark
    property var clickedCall: null
    property var itemModel: []
    model: ListModel{}

    onItemModelChanged: {
        drawModel()
    }

//    Component.onCompleted: {
//        drawModel()
//    }

    function drawModel(){
        listView.model.clear()
        if(itemModel.length > 0){
            for(var i = 0; i < itemModel.length; i++){
                listView.model.append(itemModel[i])
            }
        }
        currentIndex = currentIndex
    }


    delegate: ItemDelegate {
        id: itemDelegate
        width: (element.width + 20) < 70 ? 70 : (element.width + 20)
        height: listView.height
        highlighted: ListView.isCurrentItem
        Item {
            anchors.fill: parent
            FluText {
                id: element
                text: name
                //font.bold: itemDelegate.highlighted ? true : false
                color: itemDelegate.highlighted ? highlightedColor : "#24292e"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                Behavior on color { ColorAnimation {duration: 100} }
            }
        }

        onClicked: {
            listView.currentIndex = index
            if(clickedCall != null){
                clickedCall(index)
            }
            if(typeof(exp) !== "undefined"){
                callback()
            }
        }
        background: Rectangle {
            color: "transparent"
            Rectangle {
                width: parent.width
                height: 2
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                color:  itemDelegate.highlighted ? highlightedColor : "transparent"
            }
        }
    }


}
