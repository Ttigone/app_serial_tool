/*
 * Note:this file base on Qt Company.
 * if you want to use this file, you
 * need configure font fontawesome-webfont.ttf
 * in your project and some icon will be show normal.
 *
*/

import QtQuick 2.12
import Qt.labs.folderlistmodel 2.12
import FluentUI 1.0

Rectangle {
    id: fileBrowser

    property int buttonHeight: parent.height / 15

    // 当前文件夹路径
    property string folder
    property int itemHeight: (parent.height - buttonHeight) / 15
    property string nameFilters: "*.*"
    property string root_path: rootDirPath

    // 选择文件信号
    signal fileSelected(string file)

    function close() {
        loader.sourceComponent = undefined;

        fileBrowser.visible = false;
        fileBrowser.opacity = 0;
    }
    function open() {
        loader.sourceComponent = fileBrowserComponent;
        loader.item.parent = parent;
        loader.item.anchors.fill = parent;
        loader.item.folder = fileBrowser.folder;

        console.log(fileBrowser.folder);

        fileBrowser.visible = true;
        fileBrowser.opacity = 1;
    }
    function selectFile(file) {
        if (file !== "") {
            folder = loader.item.folders.folder;
            fileBrowser.fileSelected(file);
        }

        close();
    }

    color: Qt.rgba(0, 0, 0, 0.3)
    visible: false

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {}
        onPressed: {}
        onReleased: {}
    }
    Loader {
        id: loader

    }
    Component {
        id: fileBrowserComponent

        Rectangle {
            id: fileBrowserRoot

            property alias folder: folders1.folder
            property variant folders: folders1
            property bool showFocusHighlight: false
            property color textColor: FluColors.Grey220
            property variant view: view1

            function down(path) {
                if (folders == folders1) {
                    view = view2;
                    folders = folders2;
                    view1.state = "exitLeft";
                } else {
                    view = view1;
                    folders = folders1;
                    view2.state = "exitLeft";
                }
                view.x = fileBrowserRoot.width;
                view.state = "current";
                view.focus = true;
                folders.folder = path;
            }
            function keyPressed(key) {
                switch (key) {
                case Qt.Key_Up:
                case Qt.Key_Down:
                case Qt.Key_Left:
                case Qt.Key_Right:
                    fileBrowserRoot.showFocusHighlight = true;
                    break;
                default:
                    // do nothing
                    break;
                }
            }
            function up() {
                var path = folders.parentFolder;
                var cur_path = folders.folder;
                console.log("up", path);
                console.log("cur", cur_path);
                console.log("root_path", root_path);

                if (path.toString().length === 0 || cur_path.toString() === root_path)
                    return;

                if (folders == folders1) {
                    view = view2;
                    folders = folders2;
                    view1.state = "exitRight";
                } else {
                    view = view1;
                    folders = folders1;
                    view2.state = "exitRight";
                }
                view.x = -fileBrowserRoot.width;
                view.state = "current";
                view.focus = true;
                folders.folder = path;
            }

            //anchors.fill: parent

            color: FluColors.White

            Keys.onPressed: {
                fileBrowserRoot.keyPressed(event.key);
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Select || event.key === Qt.Key_Right) {
                    view.currentItem.launch();
                    event.accepted = true;
                } else if (event.key === Qt.Key_Left) {
                    up();
                }
            }

            FolderListModel {
                id: folders1

                folder: folder
                nameFilters: fileBrowser.nameFilters
                showDirsFirst: true
            }
            FolderListModel {
                id: folders2

                folder: folder
                nameFilters: fileBrowser.nameFilters
                showDirsFirst: true
            }
            SystemPalette {
                id: palette

            }
            Component {
                id: folderDelegate

                Rectangle {
                    id: wrapper

                    function launch() {
                        var path = "file://";
                        if (filePath.length > 2 && filePath[1] === ':') // Windows drive logic, see QUrl::fromLocalFile()
                            path += '/';
                        path += filePath;
                        if (folders.isFolder(index))
                            down(path);
                        else
                            fileBrowser.selectFile(path);
                    }

                    color: FluColors.White
                    height: folderImage.height
                    width: fileBrowserRoot.width

                    states: [
                        State {
                            name: "pressed"
                            when: mouseRegion.pressed

                            PropertyChanges {
                                target: highlight
                                visible: true
                            }
                            PropertyChanges {
                                color: palette.highlightedText
                                target: nameText
                            }
                        }
                    ]

                    Rectangle {
                        id: highlight

                        anchors.fill: parent
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        color: FluTheme.primaryColor.dark
                        visible: false
                    }
                    Item {
                        id: folderImage

                        height: itemHeight
                        width: itemHeight

                        Image {
                            id: folderPicture

                            anchors.left: parent.left
                            anchors.margins: 5
                            height: itemHeight * 0.9
                            visible: folders.isFolder(index)
                            width: itemHeight * 0.9

                            Text {
                                anchors.fill: parent
                                color: FluColors.Grey220
                                font.family: "FontAwesome"
                                horizontalAlignment: Text.AlignHCenter
                                text: "\uf115"
                                verticalAlignment: Text.AlignVCenter

                                Component.onCompleted: font.pointSize = parent.width / 2
                            }
                        }
                        Image {
                            anchors.left: parent.left
                            anchors.margins: 5
                            height: itemHeight * 0.9
                            visible: !folders.isFolder(index)
                            width: itemHeight * 0.9

                            Text {
                                anchors.fill: parent
                                color: FluColors.Grey220
                                font.family: "FontAwesome"
                                horizontalAlignment: Text.AlignHCenter
                                text: "\uf0f6"
                                verticalAlignment: Text.AlignVCenter

                                Component.onCompleted: font.pointSize = parent.width / 2
                            }
                        }
                    }
                    Text {
                        id: nameText

                        anchors.fill: parent
                        anchors.leftMargin: itemHeight + 10
                        color: (wrapper.ListView.isCurrentItem && fileBrowserRoot.showFocusHighlight) ? palette.highlightedText : textColor
                        elide: Text.ElideRight
                        text: fileName
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WrapAnywhere
                    }
                    MouseArea {
                        id: mouseRegion

                        anchors.fill: parent

                        onClicked: {
                            if (folders === wrapper.ListView.view.model)
                                launch();
                        }
                        onPressed: {
                            fileBrowserRoot.showFocusHighlight = false;
                            wrapper.ListView.view.currentIndex = index;
                        }
                    }
                }
            }
            ListView {
                id: view1

                anchors.bottom: cancelButton.top
                anchors.top: titleBar.bottom
                delegate: folderDelegate
                focus: true
                highlightMoveVelocity: 1000
                model: folders1
                pressDelay: 100
                state: "current"
                width: parent.width

                highlight: Rectangle {
                    color: FluTheme.primaryColor.dark
                    visible: fileBrowserRoot.showFocusHighlight && view1.count != 0
                    width: view1.currentItem == null ? 0 : view1.currentItem.width
                }
                states: [
                    State {
                        name: "current"

                        PropertyChanges {
                            target: view1
                            x: 0
                        }
                    },
                    State {
                        name: "exitLeft"

                        PropertyChanges {
                            target: view1
                            x: -fileBrowserRoot.width
                        }
                    },
                    State {
                        name: "exitRight"

                        PropertyChanges {
                            target: view1
                            x: fileBrowserRoot.width
                        }
                    }
                ]
                transitions: [
                    Transition {
                        to: "current"

                        SequentialAnimation {
                            NumberAnimation {
                                duration: 250
                                properties: "x"
                            }
                        }
                    },
                    Transition {
                        NumberAnimation {
                            duration: 250
                            properties: "x"
                        }
                        NumberAnimation {
                            duration: 250
                            properties: "x"
                        }
                    }
                ]

                Keys.onPressed: fileBrowserRoot.keyPressed(event.key)
            }
            ListView {
                id: view2

                anchors.bottom: parent.bottom
                anchors.top: titleBar.bottom
                delegate: folderDelegate
                highlightMoveVelocity: 1000
                model: folders2
                pressDelay: 100
                width: parent.width
                x: parent.width

                highlight: Rectangle {
                    color: FluTheme.primaryColor.dark
                    visible: fileBrowserRoot.showFocusHighlight && view2.count != 0
                    width: view1.currentItem == null ? 0 : view1.currentItem.width
                }
                states: [
                    State {
                        name: "current"

                        PropertyChanges {
                            target: view2
                            x: 0
                        }
                    },
                    State {
                        name: "exitLeft"

                        PropertyChanges {
                            target: view2
                            x: -fileBrowserRoot.width
                        }
                    },
                    State {
                        name: "exitRight"

                        PropertyChanges {
                            target: view2
                            x: fileBrowserRoot.width
                        }
                    }
                ]
                transitions: [
                    Transition {
                        to: "current"

                        SequentialAnimation {
                            NumberAnimation {
                                duration: 250
                                properties: "x"
                            }
                        }
                    },
                    Transition {
                        NumberAnimation {
                            duration: 250
                            properties: "x"
                        }
                    }
                ]

                Keys.onPressed: fileBrowserRoot.keyPressed(event.key)
            }
            Rectangle {
                anchors.bottom: parent.bottom
                color: "#ffffff"
                height: buttonHeight + 10
                width: parent.width
            }
            Rectangle {
                id: cancelButton

                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: 5
                anchors.right: parent.right
                color: FluTheme.primaryColor.dark
                height: buttonHeight
                radius: buttonHeight / 15
                width: parent.width

                FluText {
                    anchors.fill: parent
                    //color: FluColors.White
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Cancel")
                    verticalAlignment: Text.AlignVCenter
                }
                MouseArea {
                    anchors.fill: parent

                    onClicked: fileBrowser.selectFile("")
                }
            }
            Rectangle {
                id: titleBar

                anchors.top: parent.top
                color: FluColors.White
                height: buttonHeight + 10
                width: parent.width

                Rectangle {
                    anchors.left: parent.left
                    anchors.margins: 5
                    anchors.right: parent.right
                    anchors.top: parent.top
                    color: FluTheme.primaryColor.dark
                    height: buttonHeight
                    radius: buttonHeight / 15
                    width: parent.width

                    Rectangle {
                        id: upButton

                        color: "transparent"
                        height: buttonHeight
                        width: buttonHeight

                        states: [
                            State {
                                name: "pressed"
                                when: upRegion.pressed

                                PropertyChanges {
                                    color: palette.highlight
                                    target: upButton
                                }
                            }
                        ]

                        Image {
                            anchors.centerIn: parent
                            height: itemHeight
                            width: itemHeight

                            Text {
                                anchors.fill: parent
                                color: FluColors.Grey220
                                font.family: "FontAwesome"
                                horizontalAlignment: Text.AlignHCenter
                                text: "\uf112"
                                verticalAlignment: Text.AlignVCenter

                                Component.onCompleted: font.pointSize = parent.width / 2
                            }
                        }
                        MouseArea {
                            id: upRegion

                            anchors.centerIn: parent
                            height: buttonHeight
                            width: buttonHeight

                            onClicked: up()
                        }
                    }
                    Text {
                        anchors.left: upButton.right
                        anchors.leftMargin: 5
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        color: FluColors.Grey220
                        elide: Text.ElideLeft
                        height: parent.height
                        horizontalAlignment: Text.AlignLeft
                        text: folders.folder
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}
