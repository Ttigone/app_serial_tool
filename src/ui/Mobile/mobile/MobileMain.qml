import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import FluentUI 1.0

Window {
    id: window

    function hideLoadingDialog() {
        loadingDialog.close();
    }
    function showCustomSpeed(title, param_id) {
        customSpeedDialog.title = title;
        customSpeedDialog.refreshParamID(param_id);

        //加速度
        if (param_id === 0x0140 || param_id === 0x0141 || param_id === 0x0142) {
            customSpeedDialog.setXYLabel("Speed(%)", "Acc(%)");
        } else {
            customSpeedDialog.setXYLabel("Pwm(%)", "Speed(%)");
        }

        customSpeedDialog.open();
    }
    function showError(text, duration, moremsg) {
        infoBar.showError(text, duration, moremsg);
    }
    function showInfo(text, duration, moremsg) {
        infoBar.showInfo(text, duration, moremsg);
    }
    function showLoadingDialog(loading_text) {
        loadingDialog.loadingText = loading_text;
        loadingDialog.open();
    }
    function showMainPage() {
        stackLayout.currentIndex = 0;
    }
    function showMessageDialog(title, message) {
        messageDialog.title = title;
        messageDialog.message = message;
        messageDialog.open();
    }
    function showPage(index) {
        stackLayout.currentIndex = index;
    }
    function showSuccess(text, duration, moremsg) {
        infoBar.showSuccess(text, duration, moremsg);
    }
    function showWarning(text, duration, moremsg) {
        infoBar.showWarning(text, duration, moremsg);
    }

    color: "transparent"
    height: 960
    title: "Sinemotion Tool" + " (Build Time:" + Config.getBuildDateTime() + ")"
    visible: true
    width: 540

    Component.onCompleted: {}

    Rectangle {
        id: root

        anchors.fill: parent
        color: "white"

        //堆叠窗口结构
        StackLayout {
            id: stackLayout

            anchors.fill: parent
            currentIndex: 0

            MainPageM {
                id: mainPage

            }
            EscSettingPageM {
                id: escSettingPage

            }
            RealtimeDataPageM {
                id: realtimeDataPage

            }
            HistoryDataPageM {
                id: historyDataPage

            }
            AnalysisPageM {
                id: errorAnalysisPage

            }
            FirmwarePageM {
                id: firmwarePage

            }
            SettingPageM {
                id: sysSettingPage

            }
        }
        FluContentDialog {
            id: messageDialog

            buttonFlags: FluContentDialogType.PositiveButton
            message: ""
            parent: root
            positiveText: "OK"
            title: "Note Info"

            onPositiveClicked: {}
        }
        ParamCustomSpeedDialogM {
            id: customSpeedDialog

            parent: root
            title: "Custom Speed"
        }
        LoadingDialogM {
            id: loadingDialog

            loadingText: "Loading"
            parent: root
        }
        FluInfoBar {
            id: infoBar

            root: window
        }
    }
    Connections {
        target: AppInterface

        onMessageDialog: {
            messageDialog.title = title;
            messageDialog.message = msg;
            messageDialog.open();
        }
    }
}
