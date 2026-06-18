import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.0

import FluentUI 1.0
import QmlCustomPlot 1.0

Item {
    id:page

    property int testingDeviceID: 0
    property int testingDeviceIndex: 0
    property var testDeviceList : new Array
    property int testCurrentTimes: 0
    property int testMaxTimes: 1

    property int pwmStep: 40

    property int maxSpeed: 10000
    property int speedStep: 100

    property string sn_code: ""
    property string log_date_time: ""

    Component.onCompleted: {
    }


    onVisibleChanged: {

        if(visible)
        {
            console.log("enter Realtime")
            refreshPlot();

        }
        else
        {
            console.log("leave Realtime")

            stopTest(0);
        }
    }

    function initPage(){
    }

    function startTest(){

        showLoadingDialog(qsTr("Start Testing..."));

        startTestButton.text = qsTr("Testing")
        startTestButton.enabled = false;

        RealtimeDataManager.resetWave();
        TestDataManager.setDataSaveFlag(false);

        testingDeviceID = ParamManager.getEscID();

        escIDLabel.text = "ESC ID :" + testingDeviceID.toString();

        var current_date = new Date();
        log_date_time =current_date.toLocaleString(Qt.locale(), "hh-mm-ss");

        if(Command.queryESCInfo(testingDeviceID))
        {
            var esc_info = ParamManager.getEscInfoParam();
            sn_code = esc_info.sn_code;

            snLabel.text = "SN : " + esc_info.sn_code;

            testStateLabel.color = FluTheme.dark ? FluColors.White : FluColors.Grey220

            hideLoadingDialog();

            rt_timer.start();
            refreshPlot();


        }
        else
        {
            hideLoadingDialog();

            startTestButton.text = qsTr("Test");
            startTestButton.enabled = true;

            AppInterface.emitMessageDialog(qsTr("Error"), qsTr("Query SN Code Failure, Please check ESC Node ID and CAN Comm, or retry start test."));
        }
    }

    function stopTest(){
        startTestButton.text = qsTr("Test")
        startTestButton.enabled = true;

        testStateLabel.text = qsTr("State : Idle");
        testStateLabel.color = FluTheme.dark ? FluColors.White : FluColors.Grey220

        if(rt_timer.running === false)
            return

        rt_timer.stop();

        TestDataManager.saveData(sn_code, log_date_time, true);
        TestDataManager.setDataSaveFlag(true);
    }

    function refreshPlot(){
        if(time10msCheckBox.checked)
        {
            dataView.setWaveVisiable("time_10ms", true);
        }
        else
        {
            dataView.setWaveVisiable("time_10ms", false);
        }

        if(recvPwmCheckBox.checked)
        {
            dataView.setWaveVisiable("recv_pwm", true);
        }
        else
        {
            dataView.setWaveVisiable("recv_pwm", false);
        }

        if(commPwmCheckBox.checked)
        {
            dataView.setWaveVisiable("comm_pwm", true);
        }
        else
        {
            dataView.setWaveVisiable("comm_pwm", false);
        }

        if(speedCheckBox.checked)
        {
            dataView.setWaveVisiable("speed", true);
        }
        else
        {
            dataView.setWaveVisiable("speed", false);
        }

        if(currentCheckBox.checked)
        {
            dataView.setWaveVisiable("current", true);
        }
        else
        {
            dataView.setWaveVisiable("current", false);
        }

        if(iBusCheckBox.checked)
        {
            dataView.setWaveVisiable("bus_current", true);
        }
        else
        {
            dataView.setWaveVisiable("bus_current", false);
        }

        if(voltageCheckBox.checked)
        {
            dataView.setWaveVisiable("voltage", true);
        }
        else
        {
            dataView.setWaveVisiable("voltage", false);
        }

        if(vModultagitonCheckBox.checked)
        {
            dataView.setWaveVisiable("v_modulation", true);
        }
        else
        {
            dataView.setWaveVisiable("v_modulation", false);
        }

        if(mosTempCheckBox.checked)
        {
            dataView.setWaveVisiable("mos_temp", true);
        }
        else
        {
            dataView.setWaveVisiable("mos_temp", false);
        }


        if(capTempCheckBox.checked)
        {
            dataView.setWaveVisiable("cap_temp", true);
        }
        else
        {
            dataView.setWaveVisiable("cap_temp", false);
        }

        if(mcuTempCheckBox.checked)
        {
            dataView.setWaveVisiable("mcu_temp", true);
        }
        else
        {
            dataView.setWaveVisiable("mcu_temp", false);
        }

        if(motorTempCheckBox.checked)
        {
            dataView.setWaveVisiable("motor_temp", true);
        }
        else
        {
            dataView.setWaveVisiable("motor_temp", false);
        }

        if(runningErrorCheckBox.checked)
        {
            dataView.setWaveVisiable("running_error", true);
        }
        else
        {
            dataView.setWaveVisiable("running_error", false);
        }

        if(selfCheckErrorCheckBox.checked)
        {
            dataView.setWaveVisiable("selfcheck_error", true);
        }
        else
        {
            dataView.setWaveVisiable("selfcheck_error", false);
        }

        dataView.updatePlot();

        TestDataManager.saveData(sn_code, log_date_time, false);
    }

    Timer {
        id: rt_timer
        interval: 50
        repeat: true
        running: false
        onTriggered: {
            Command.queryRealtimeData(testingDeviceID);
            dataView.updatePlot();
        }
    }

    ColumnLayout
    {
        id:container
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 10

        clip: true

        Rectangle{
            id:header
            color: FluTheme.primaryColor.dark
            Layout.fillWidth: true
            implicitHeight : 50

            FluIconButton {
                id: home
                anchors.left: header.left
                anchors.top: header.top
                anchors.rightMargin: 8
                width: 50
                height: 50
                iconSize:35
                iconSource:FluentIcons.Back
                iconColor: FluColors.White

                onClicked: {
                    stopTest();
                    showMainPage();
                }
            }

            FluText {
                id:title_label
                text: qsTr("Realtime Data")
                font.pixelSize: 24
                color: FluColors.White
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter : parent.verticalCenter
            }
        }

        RowLayout{
            id:buttonLayout
            Layout.leftMargin: 10
            spacing: 10
            FluButton{
                id:startTestButton
                text: qsTr("Test")
                Layout.preferredWidth: 80
                onClicked:
                {
                    if(!BleUart.isConnected())
                    {
                        showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                        return;
                    }
                    startTest();
                }
            }

            FluButton{
                id:stopTestButton
                text: qsTr("Stop")
                Layout.preferredWidth: 80

                onClicked:
                {
                    stopTest();
                }
            }

            FluText{
                id:testStateLabel
                text: qsTr("State : Idle")
                font : FluTextStyle.Subtitle
            }
        }
        GridLayout
        {
            Layout.leftMargin: 5
            columns: 2
            columnSpacing:0
            rowSpacing: 5

            FluText {
                id:speedKmLabel
                text: "Speed(km/h) :"
                Layout.columnSpan: 2
                font.pixelSize: 24
            }

            FluText{
                id:escIDLabel
                Layout.preferredWidth: (page.width-10)/2
                text:"ESC ID :"
            }

            FluText{
                id:snLabel
                Layout.preferredWidth: (page.width-40)/2
                text:"SN :"
            }

            FluCheckBox{
                id:time10msCheckBox
                text:"Time 10ms :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave1
            }

            FluCheckBox{
                id:recvPwmCheckBox
                text:"RecvPWM(us) :"
                checked: true
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave2
            }

            FluCheckBox{
                id:commPwmCheckBox
                text:"CommPWM(us) :"
                checked: true
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave3
            }

            FluCheckBox{
                id:speedCheckBox
                text:"Speed(rpm) :"
                checked: true
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave4
            }

            FluCheckBox{
                id:currentCheckBox
                text:"Current(A) :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave5
            }

            FluCheckBox{
                id:iBusCheckBox
                text:"Bus Current(A) :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave6

            }

            FluCheckBox{
                id:voltageCheckBox
                text:"Voltage(V) :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave7
            }

            FluCheckBox{
                id:vModultagitonCheckBox
                text:"V Modultagiton :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave8
            }

            FluCheckBox{
                id:mosTempCheckBox
                text:"Mos Temp(°C) :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave9
            }

            FluCheckBox{
                id:capTempCheckBox
                text:"Cap Temp(°C) :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave10
            }

            FluCheckBox{
                id:mcuTempCheckBox
                text:"Mcu Temp(°C) :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave11
            }

            FluCheckBox{
                id:motorTempCheckBox
                text:"Motor Temp(°C) :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave12
            }

            FluCheckBox{
                id:runningErrorCheckBox
                text:"Running Error :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave13
            }

            FluCheckBox{
                id:selfCheckErrorCheckBox
                text:"Selfcheck Error :"
                onClicked: {
                    refreshPlot();
                }
                checkedColor:FluColors.Wave14
            }

        }


        RowLayout{
            spacing: 10
            Layout.leftMargin: 10

            FluButton{
                id:resetScaleButton
                width: height

                Image {
                    width: 24
                    height: 24
                    source: "qrc:/Resource/icon/adjust.png"
                    fillMode: Image.PreserveAspectFit
                    anchors{
                        centerIn:  parent
                    }
                }

                onClicked:
                {
                    dataView.resaclePlot();
                }
            }

            FluToggleButton{
                id:showPointButton
                width: height
                Image {
                    width: 24
                    height: 24
                    source: "qrc:/Resource/icon/sample_point.png"
                    anchors{
                        centerIn:  parent
                    }
                }

                onCheckedChanged:
                {
                    if(checked)
                    {
                        dataView.showDataPoint(true);
                    }
                    else
                    {
                        dataView.showDataPoint(false);
                    }
                }
            }
        }

        QmlCustomPlot{
            id:dataView
            Layout.minimumWidth: 100
            Layout.minimumHeight: 100

            Layout.fillWidth: true
            Layout.fillHeight: true

            Component.onCompleted: {
                initCustomPlot();
                setLegendVisiable(false);
                updateCustomPlotSize();
                setXLabel("Time(s)");

                addRealWave("time_10ms", "Time 10ms");
                setColor("time_10ms", FluColors.Wave1);

                addRealWave("recv_pwm", "Recv Pwm");
                setColor("recv_pwm", FluColors.Wave2);

                addRealWave("comm_pwm", "Comm Pwm");
                setColor("comm_pwm", FluColors.Wave3);

                addRealWave("speed", "Speed(rpm)");
                setColor("speed", FluColors.Wave4);

                addRealWave("current", "Current)");
                setColor("current", FluColors.Wave5);

                addRealWave("bus_current", "Bus Current");
                setColor("bus_current", FluColors.Wave6);

                addRealWave("voltage", "Voltage");
                setColor("voltage", FluColors.Wave7);

                addRealWave("v_modulation", "V Modulation");
                setColor("v_modulation", FluColors.Wave8);

                addRealWave("mos_temp", "Mos Temp");
                setColor("mos_temp", FluColors.Wave9);

                addRealWave("cap_temp", "Cap Temp");
                setColor("cap_temp", FluColors.Wave10);

                addRealWave("mcu_temp", "Mcu Temp");
                setColor("mcu_temp", FluColors.Wave11);

                addRealWave("motor_temp", "Motor Temp");
                setColor("motor_temp", FluColors.Wave12);

                addRealWave("running_error", "Running State");
                setColor("running_error", FluColors.Wave13);

                addRealWave("selfcheck_error", "Selfcheck Error");
                setColor("selfcheck_error", FluColors.Wave14);
            }
        }
    }


    Connections {
        target: Command

        onRealtimeDataReceived:{
            if(!visible)
            {
                return;
            }

            speedKmLabel.text = "Speed(km/h) : " + realtime_data.speed_km_h/10.0;
            time10msCheckBox.text = "Time 10ms : " + realtime_data.time_10ms;
            recvPwmCheckBox.text = "RecvPWM(us) : " + realtime_data.recv_pwm/10.0;
            commPwmCheckBox.text = "CommPWM(us) : " + realtime_data.comm_pwm/10.0;

            speedCheckBox.text = "Speed(rpm) : " + realtime_data.speed;

            currentCheckBox.text = "Current(A) : " + realtime_data.current/10.0;
            iBusCheckBox.text = "Bus Current(A) : " + realtime_data.bus_current/10.0;

            voltageCheckBox.text = "Voltage(V) : " + realtime_data.voltage/10.0;
            vModultagitonCheckBox.text = "V Modultagiton : " + realtime_data.v_modulation/10.0;

            mosTempCheckBox.text = "Mos Temp(°C) : " + realtime_data.mos_temp/10.0;
            capTempCheckBox.text = "Cap Temp(°C) : " + realtime_data.cap_temp/10.0;
            mcuTempCheckBox.text = "Mcu Temp(°C) : " + realtime_data.mcu_temp/10.0;
            motorTempCheckBox.text = "Motor Temp(°C) :" + realtime_data.motor_temp/10.0;

            runningErrorCheckBox.text = "Running State : " + realtime_data.running_error;
            selfCheckErrorCheckBox.text = "Selfcheck Error : " + realtime_data.selfcheck_error;
        }
    }
}
