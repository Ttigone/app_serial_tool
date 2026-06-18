import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.12
import FluentUI 1.0

Window {
    id: window
    flags: (Qt.Window |
                    Qt.FramelessWindowHint |
                    Qt.WindowSystemMenuHint |
                    Qt.WindowMinimizeButtonHint |
                    Qt.WindowMaximizeButtonHint)

    property string logoIcon: AppLogo
    property bool fristWindowLoad: true
    property int shadowWidthOrgin:10
    property color shadowColorOrgin:"#80000000"
    property int currVisi: Window.Windowed

    property bool doubleClick: false
    default property alias zbody: zbodyComponent.data
    signal zdragWindowSizeChanged()

    visibility: Window.Windowed

    Component.onCompleted: {
    }

    onActiveChanged: {
        if(active)
        {
            visibility = currVisi;
            shadowColorOrgin = "#80000000";
        }
        else
        {
            shadowColorOrgin ="transparent"
        }
    }
    onCurrVisiChanged: {
        if(currVisi == Window.Maximized){
            shadowWidthOrgin = 0;
        }else{
            shadowWidthOrgin = 10;
        }
        visibility = currVisi
    }

    Rectangle{
        id:shadowRect
        anchors.fill: parent
        anchors.margins: shadowWidthOrgin - 1
        border.color: FluTheme.primaryColor.dark
        border.width: 1
        Behavior on border.color { ColorAnimation {duration: 200} }

        MouseArea{
            enabled: shadowWidthOrgin > 0
            hoverEnabled: true
            cursorShape: Qt.SizeFDiagCursor
            anchors.right: parent.right
            anchors.rightMargin: -shadowWidthOrgin / 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -shadowWidthOrgin / 2
            width: shadowWidthOrgin
            height: width
            property point startPos: Qt.point(0,0)
            property point offsetPos: Qt.point(0,0)

            onPressed: startPos = Qt.point(mouseX , mouseY)
            onPositionChanged: {
                if(pressed){
                    offsetPos = Qt.point(mouseX - startPos.x, mouseY - startPos.y)
                    var afterWidth = window.width + offsetPos.x
                    var afterHeight = window.height + offsetPos.y
                    if(afterWidth > window.minimumWidth){
                        window.width = afterWidth
                    }
                    if(afterHeight > window.minimumHeight){
                        window.height = afterHeight
                    }
                }
            }
            onReleased: zdragWindowSizeChanged()
        }
    }

    DropShadow {
        anchors.fill: shadowRect
        radius: shadowWidthOrgin * 2 - 2
        samples: 41
        color: shadowColorOrgin
        source: shadowRect
    }

    function savePos(){
        dragArea.windowLastPos.x = window.x;dragArea.windowLastPos.y = window.y;
    }

    Rectangle{
        id: titleRect
        anchors{
            top: parent.top
            topMargin: shadowWidthOrgin
            left: parent.left
            leftMargin: shadowWidthOrgin
            right: parent.right
            rightMargin: shadowWidthOrgin
        }

        height: 35
        z:999
        color: FluTheme.primaryColor.dark

        Behavior on color { ColorAnimation {duration: 200} }

        Image {
            id: iconImg
            width: 24
            height: 24
            source: logoIcon
            anchors{
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }
        }

        FluText{
            text: window.title
            color: "white"
            anchors.left: iconImg.right
            anchors.leftMargin: 5
            anchors.verticalCenter: parent.verticalCenter
        }


        Rectangle{
            z: 999
            width: 35
            color: hovered ? Qt.darker(FluTheme.primaryColor.dark, 1.3) : "transparent"
            property alias hovered: minimizeMouseArea.containsMouse
            anchors{
                right: maximizeRect.left
                top: parent.top
                bottom: parent.bottom
            }
            MouseArea{
                id: minimizeMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    window.showMinimized()
                }
            }

            FluIcon {
                id: minimizeImg
                width: 24
                height: 24
                iconSource : FluentIcons.ChromeMinimize
                iconColor: FluColors.White
                anchors{
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
            }
        }

        Rectangle{
            id: maximizeRect
            z: 999
            width: 35
            color: hovered ? Qt.darker(FluTheme.primaryColor.dark, 1.3) : "transparent"
            property alias hovered: maximizeMouseArea.containsMouse
            anchors{
                right: closeRect.left
                top: parent.top
                bottom: parent.bottom
            }
            MouseArea{
                id: maximizeMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    if(currVisi === Window.Windowed){
                        savePos()
                        currVisi = Window.Maximized
                    }else{
                        currVisi = Window.Windowed
                        window.x = dragArea.windowLastPos.x
                        window.y = dragArea.windowLastPos.y
                    }
                }
            }


            FluIcon {
                id: maximizeImg
                width: 24
                height: 24
                iconSource : currVisi === Window.Maximized ? FluentIcons.ChromeRestore : FluentIcons.ChromeMaximize
                iconColor: FluColors.White
                anchors{
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
            }
        }

        Rectangle{
            id: closeRect
            z: 999
            width: 35
            color: hovered ? Qt.darker(FluTheme.primaryColor.dark, 1.3) : "transparent"
            property alias hovered: closeMouseArea.containsMouse
            anchors{
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            MouseArea{
                id: closeMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    window.close()
                }
            }

            FluIcon {
                id: closeImg
                width: 24
                height: 24
                iconSource : FluentIcons.ChromeClose
                iconColor: FluColors.White
                anchors{
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
            }
        }

        Timer{
            id: timer
            interval: 200
            running: false
            repeat: false
            onTriggered: {
                doubleClick = false
            }
        }

        MouseArea {
            id: dragArea
            anchors.fill: parent
            property point startPos: Qt.point(0,0)
            property point offsetPos: Qt.point(0,0)
            property point windowLastPos: Qt.point(0,0)
            property bool preMax: false

            // 全屏状态下拖动
            property point maxStartPos: Qt.point(0,0)
            // 全屏到窗口位置是否需要重设
            property bool maxToWinRedirect: false

            onPressed: {
                if(currVisi === Window.Maximized)
                {
                    maxStartPos = Qt.point(mouseX,mouseY);maxToWinRedirect = true;
                    return;
                }

                savePos()
                startPos = Qt.point(mouseX , mouseY)
            }
            onReleased: {
                if(window.y < -1 * shadowWidthOrgin){window.y = -1 * shadowWidthOrgin}
                if(preMax)
                {
                    shadowWidthOrgin = 0;
                    currVisi = Window.Maximized;
                    preMax = false;
                }
            }

            onPositionChanged: {
                // 解决高分屏由于像素抖动双击无法全屏的问题
                if(doubleClick){return}
                if(currVisi === Window.Maximized){currVisi = Window.Windowed}
                preMax = window.y + mouse.y == -1 * shadowWidthOrgin

                if(maxToWinRedirect){
                    var stll = (Screen.desktopAvailableWidth - window.width) / 2
                    if (mouse.x < stll){
                        // 靠左上角
                        window.x = 0
                        window.y = 0
                        startPos = Qt.point(maxStartPos.x,maxStartPos.y)
                    }else if(mouse.x > Screen.desktopAvailableWidth - stll){
                        // 靠右上角
                        window.x = Screen.desktopAvailableWidth - window.width
                        window.y = 0
                        startPos = Qt.point(window.width - (Screen.desktopAvailableWidth - maxStartPos.x),maxStartPos.y)
                    }else{
                        // 中心靠鼠标
                        window.x = maxStartPos.x - (window.width / 2) + -shadowWidthOrgin
                        window.y = -1 * shadowWidthOrgin
                        startPos = Qt.point(window.width / 2,maxStartPos.y)
                    }
                    maxToWinRedirect = false
                    return
                }
                offsetPos = Qt.point(mouseX - startPos.x, mouseY - startPos.y)
                window.x += offsetPos.x
                window.y += offsetPos.y
            }

            onDoubleClicked: {
                doubleClick = true
                if(currVisi === Window.Windowed){
                    timer.start()
                    currVisi = Window.Maximized
                }else{
                    timer.start()
                    // 取消掉全屏的拖动处理
                    maxToWinRedirect = false
                    currVisi = Window.Windowed
                    window.x = windowLastPos.x
                    window.y = windowLastPos.y
                }
            }
        }
    }
    Item {
        id: zbodyComponent
        anchors.fill: parent
        anchors.topMargin: 35 + shadowWidthOrgin
        anchors.leftMargin: shadowWidthOrgin
        anchors.rightMargin: shadowWidthOrgin
        anchors.bottomMargin: shadowWidthOrgin
    }
}
