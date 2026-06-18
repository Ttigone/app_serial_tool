import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

import FluentUI 1.0
import QmlCustomEditPlot 1.0

FluPopup {
    id: popup

    property var minWidth: {
        if (Window.window == null) {
            return 300;
        }
        return Math.min(Window.window.width, 300);
    }
    property string title: "Custom Speed"

    function refreshParamID(param_id) {
        customSpeedPlot.setParamID(param_id);
    }
    function setXYLabel(x_label, y_label) {
        customSpeedPlot.setLable(x_label, y_label);
    }

    focus: true

    Rectangle {
        id: layout_content

        anchors.fill: parent
        color: 'transparent'
        implicitHeight: text_title.height + customSpeedPlot.height + layout_actions.height
        implicitWidth: minWidth
        radius: 5

        FluText {
            id: text_title

            font: FluTextStyle.Subtitle
            leftPadding: 20
            rightPadding: 20
            text: title
            topPadding: 20
            wrapMode: Text.WrapAnywhere

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
        }
        QmlCustomEditPlot {
            id: customSpeedPlot

            height: 300

            Component.onCompleted: {
                initCustomPlot();
                updateCustomPlotSize();

                setLable("Pwm(%)", "Speed(%)");
            }

            anchors {
                left: parent.left
                right: parent.right
                top: text_title.bottom
            }
        }
        Rectangle {
            id: layout_actions

            color: FluTheme.dark ? Qt.rgba(32 / 255, 32 / 255, 32 / 255, blurBackground ? blurOpacity - 0.4 : 1) : Qt.rgba(243 / 255, 243 / 255, 243 / 255, blurBackground ? blurOpacity - 0.4 : 1)
            height: 68
            radius: 5

            anchors {
                left: parent.left
                right: parent.right
                top: customSpeedPlot.bottom
            }
            RowLayout {
                spacing: 15

                anchors {
                    centerIn: parent
                    fill: parent
                    margins: spacing
                }
                FluButton {
                    id: cancel_btn

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Cancel"

                    onClicked: {
                        popup.close();
                    }
                }
                FluFilledButton {
                    id: apply_btn

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Apply"

                    onClicked: {
                        customSpeedPlot.acceptCurveData();
                        popup.close();
                    }
                }
            }
        }
    }
}
