import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.0

import QtQuick.Controls 1.4

import FluentUI 1.0
import QmlCustomPlot 1.0

Item {
    id: page

    property string item_sn_str: ""
    property string item_time_str: ""

    function initPage() {
        var testDataList = TestDataManager.getTestDataDesc();

        testCaseModel.clear();

        for (var index in testDataList) {
            var test_data_item = testDataList[index];
            testCaseModel.append(test_data_item);
        }
    }
    function refreshPlot() {
        if (time10msCheckBox.checked) {
            dataView.setWaveVisiable("time_10ms", true);
        } else {
            dataView.setWaveVisiable("time_10ms", false);
        }

        if (recvPwmCheckBox.checked) {
            dataView.setWaveVisiable("recv_pwm", true);
        } else {
            dataView.setWaveVisiable("recv_pwm", false);
        }

        if (commPwmCheckBox.checked) {
            dataView.setWaveVisiable("comm_pwm", true);
        } else {
            dataView.setWaveVisiable("comm_pwm", false);
        }

        if (speedCheckBox.checked) {
            dataView.setWaveVisiable("speed", true);
        } else {
            dataView.setWaveVisiable("speed", false);
        }

        if (currentCheckBox.checked) {
            dataView.setWaveVisiable("current", true);
        } else {
            dataView.setWaveVisiable("current", false);
        }

        if (iBusCheckBox.checked) {
            dataView.setWaveVisiable("bus_current", true);
        } else {
            dataView.setWaveVisiable("bus_current", false);
        }

        if (voltageCheckBox.checked) {
            dataView.setWaveVisiable("voltage", true);
        } else {
            dataView.setWaveVisiable("voltage", false);
        }

        if (vModultagitonCheckBox.checked) {
            dataView.setWaveVisiable("v_modulation", true);
        } else {
            dataView.setWaveVisiable("v_modulation", false);
        }

        if (mosTempCheckBox.checked) {
            dataView.setWaveVisiable("mos_temp", true);
        } else {
            dataView.setWaveVisiable("mos_temp", false);
        }

        if (capTempCheckBox.checked) {
            dataView.setWaveVisiable("cap_temp", true);
        } else {
            dataView.setWaveVisiable("cap_temp", false);
        }

        if (mcuTempCheckBox.checked) {
            dataView.setWaveVisiable("mcu_temp", true);
        } else {
            dataView.setWaveVisiable("mcu_temp", false);
        }

        if (motorTempCheckBox.checked) {
            dataView.setWaveVisiable("motor_temp", true);
        } else {
            dataView.setWaveVisiable("motor_temp", false);
        }

        if (runningErrorCheckBox.checked) {
            dataView.setWaveVisiable("running_error", true);
        } else {
            dataView.setWaveVisiable("running_error", false);
        }

        if (selfCheckErrorCheckBox.checked) {
            dataView.setWaveVisiable("selfcheck_error", true);
        } else {
            dataView.setWaveVisiable("selfcheck_error", false);
        }

        if (speedKmCheckBox.checked) {
            dataView.setWaveVisiable("speed_km_h", true);
        } else {
            dataView.setWaveVisiable("speed_km_h", false);
        }

        dataView.resaclePlot();
    }

    Component.onCompleted: {}
    onVisibleChanged: {
        if (visible) {
            console.log("enter History");
            initPage();
        } else {
            console.log("leave History");
        }
    }

    ColumnLayout {
        id: container

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        clip: true
        spacing: 10

        Rectangle {
            id: header

            Layout.fillWidth: true
            color: FluTheme.primaryColor.dark
            implicitHeight: 50

            FluIconButton {
                id: home

                anchors.left: header.left
                anchors.rightMargin: 8
                anchors.top: header.top
                height: 50
                iconColor: FluColors.White
                iconSize: 35
                iconSource: FluentIcons.Back
                width: 50

                onClicked: {
                    showMainPage();
                }
            }
            FluText {
                id: title_label

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: FluColors.White
                font.pixelSize: 24
                text: qsTr("History Data")
            }
        }
        RowLayout {
            spacing: 0

            ListModel {
                id: testCaseModel

            }
            ListView {
                id: historyListView

                Layout.preferredHeight: 300
                Layout.preferredWidth: 120
                clip: true
                delegate: availableDelegate
                model: testCaseModel
                spacing: 5

                header: Rectangle {
                    border.width: 0
                    color: FluTheme.primaryColor.lightest
                    implicitHeight: 24
                    implicitWidth: historyListView.width

                    FluText {
                        anchors.centerIn: parent
                        font.pixelSize: 14
                        text: "Time"
                    }
                }
                highlight: Rectangle {
                    color: FluTheme.primaryColor.light
                    focus: true
                    opacity: 0.5
                }

                onCurrentIndexChanged: {
                    console.log("historyListView" + historyListView.currentIndex);
                    var item_desc = testCaseModel.get(historyListView.currentIndex);
                    var sn_code = item_desc['sn_code'];
                    snLabel.text = "SN : " + sn_code;

                    item_time_str = item_desc['time'];
                    item_sn_str = item_desc['sn_code'];
                    TestDataManager.parseData(item_desc['desc_path'], item_desc['data_path']);
                    refreshPlot();
                }

                Component {
                    id: availableDelegate

                    Rectangle {
                        id: cellRect

                        border.width: 0
                        color: cellRect.selected ? FluTheme.primaryColor.light : "transparent"
                        height: cellText.height
                        width: historyListView.width

                        FluText {
                            id: cellText

                            anchors.centerIn: parent
                            //elide: styleData.elideMode
                            font.pixelSize: 12
                            text: time
                        }
                        MouseArea {
                            anchors.fill: cellRect

                            onClicked: {
                                historyListView.currentIndex = index;
                            }
                        }
                    }
                }
            }
            GridLayout {
                columnSpacing: 0
                columns: 2
                rowSpacing: 5

                FluButton {
                    id: clearHistoryDataButton

                    Layout.alignment: Qt.AlignHCenter
                    Layout.columnSpan: 2
                    text: qsTr("Clear Histroy Data")

                    onClicked: {
                        deleteHistoryDataDialog.open();
                    }
                }
                FluContentDialog {
                    id: deleteHistoryDataDialog

                    buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.PositiveButton
                    message: qsTr("Are you sure to delete all history data?")
                    negativeText: qsTr("No")
                    parent: root
                    positiveText: qsTr("Yes")
                    title: qsTr("Warning")

                    onPositiveClicked: {
                        TestDataManager.clearHistoryData();
                        initPage();
                    }
                }
                FluText {
                    id: snLabel

                    Layout.columnSpan: 2
                    text: "SN :"
                }
                FluCheckBox {
                    id: time10msCheckBox

                    checkedColor: FluColors.Wave1
                    font.pixelSize: 8
                    text: "Time 10ms"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: recvPwmCheckBox

                    checked: true
                    checkedColor: FluColors.Wave2
                    font.pixelSize: 8
                    text: "RecvPWM(us)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: commPwmCheckBox

                    checked: true
                    checkedColor: FluColors.Wave3
                    font.pixelSize: 8
                    text: "CommPWM(us)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: speedCheckBox

                    checked: true
                    checkedColor: FluColors.Wave4
                    font.pixelSize: 8
                    text: "Speed(rpm)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: currentCheckBox

                    checkedColor: FluColors.Wave5
                    font.pixelSize: 8
                    text: "Current(A)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: iBusCheckBox

                    checkedColor: FluColors.Wave6
                    font.pixelSize: 8
                    text: "Bus Current(A)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: voltageCheckBox

                    checkedColor: FluColors.Wave7
                    font.pixelSize: 8
                    text: "Voltage(V)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: vModultagitonCheckBox

                    checkedColor: FluColors.Wave8
                    font.pixelSize: 8
                    text: "V Modultagiton"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: mosTempCheckBox

                    checkedColor: FluColors.Wave9
                    font.pixelSize: 8
                    text: "Mos Temp(°C)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: capTempCheckBox

                    checkedColor: FluColors.Wave10
                    font.pixelSize: 8
                    text: "Cap Temp(°C)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: mcuTempCheckBox

                    checkedColor: FluColors.Wave11
                    font.pixelSize: 8
                    text: "Mcu Temp(°C)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: motorTempCheckBox

                    checkedColor: FluColors.Wave12
                    font.pixelSize: 8
                    text: "Motor Temp(°C)"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: runningErrorCheckBox

                    checkedColor: FluColors.Wave13
                    font.pixelSize: 8
                    text: "Running State"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: selfCheckErrorCheckBox

                    checkedColor: FluColors.Wave14
                    font.pixelSize: 8
                    text: "Selfcheck Error"

                    onClicked: {
                        refreshPlot();
                    }
                }
                FluCheckBox {
                    id: speedKmCheckBox

                    checkedColor: FluColors.Wave15
                    font.pixelSize: 8
                    text: "Speed(km/h)"

                    onClicked: {
                        refreshPlot();
                    }
                }
            }
        }
        RowLayout {
            Layout.leftMargin: 10
            spacing: 10

            FluButton {
                id: resetScaleButton

                width: height

                onClicked: {
                    dataView.resaclePlot();
                }

                Image {
                    fillMode: Image.PreserveAspectFit
                    height: 24
                    source: "qrc:/Resource/icon/adjust.png"
                    width: 24

                    anchors {
                        centerIn: parent
                    }
                }
            }
            FluToggleButton {
                id: showPointButton

                width: height

                onCheckedChanged: {
                    if (checked) {
                        dataView.showDataPoint(true);
                    } else {
                        dataView.showDataPoint(false);
                    }
                }

                Image {
                    height: 24
                    source: "qrc:/Resource/icon/sample_point.png"
                    width: 24

                    anchors {
                        centerIn: parent
                    }
                }
            }
            FluButton {
                id: saveCsvButton

                width: height

                onClicked: {
                    var file_url = Config.getTestDataPath() + "/" + item_time_str + ".csv";

                    console.log(file_url);
                    TestDataManager.saveCsvFile(file_url);
                    showSuccess("Export Data File " + file_url + " Success..");

                    QtShareFile.shareFile(file_url);
                }

                Image {
                    height: 24
                    source: "qrc:/Resource/icon/save.png"
                    width: 24

                    anchors {
                        centerIn: parent
                    }
                }
            }
        }
        QmlCustomPlot {
            id: dataView

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumHeight: 100
            Layout.minimumWidth: 100

            Component.onCompleted: {
                initCustomPlot();
                setLegendVisiable(false);
                updateCustomPlotSize();
                setXLabel("Time(s)");

                addHistoryWave("time_10ms", "Time 10ms");
                setColor("time_10ms", FluColors.Wave1);

                addHistoryWave("recv_pwm", "Recv Pwm");
                setColor("recv_pwm", FluColors.Wave2);

                addHistoryWave("comm_pwm", "Comm Pwm");
                setColor("comm_pwm", FluColors.Wave3);

                addHistoryWave("speed", "Speed(rpm)");
                setColor("speed", FluColors.Wave4);

                addHistoryWave("current", "Current)");
                setColor("current", FluColors.Wave5);

                addHistoryWave("bus_current", "Bus Current");
                setColor("bus_current", FluColors.Wave6);

                addHistoryWave("voltage", "Voltage");
                setColor("voltage", FluColors.Wave7);

                addHistoryWave("v_modulation", "V Modulation");
                setColor("v_modulation", FluColors.Wave8);

                addHistoryWave("mos_temp", "Mos Temp");
                setColor("mos_temp", FluColors.Wave9);

                addHistoryWave("cap_temp", "Cap Temp");
                setColor("cap_temp", FluColors.Wave10);

                addHistoryWave("mcu_temp", "Mcu Temp");
                setColor("mcu_temp", FluColors.Wave11);

                addHistoryWave("motor_temp", "Motor Temp");
                setColor("motor_temp", FluColors.Wave12);

                addHistoryWave("running_error", "Running State");
                setColor("running_error", FluColors.Wave13);

                addHistoryWave("selfcheck_error", "Selfcheck Error");
                setColor("selfcheck_error", FluColors.Wave14);

                addHistoryWave("speed_km_h", "Speed(km/h)");
                setColor("speed_km_h", FluColors.Wave15);
            }
        }
    }
}
