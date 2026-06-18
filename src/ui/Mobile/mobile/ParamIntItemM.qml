import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import FluentUI 1.0

Item {
    id: control
    implicitHeight: 50

    property string paramID:""
    property int paramIDKey:0
    property string paramName:""

    property int paramValue:0
    property int minValue:0
    property int maxValue: 0

    property string rw_attr:"rw"


    Component.onCompleted: {
        var param_id = Number(paramID)
        paramIDKey = param_id;

        var param_info = ParamManager.getParam(param_id);

        paramName = param_info['param_name'];
        paramValue = param_info['cur_value'];
        rw_attr = param_info['rw_attr'];

        minValue = param_info['min_value'];
        maxValue = param_info['max_value'];


        valueSpinBox.from = minValue
        valueSpinBox.to = maxValue
        valueSpinBox.displayText = paramValue.toString();

        if(param_info['suffix'] === "")
            displayNameText.text = param_info['param_name'];
        else
            displayNameText.text = param_info['param_name'] + "(" + param_info['suffix'] + ")";
    }

    RowLayout{
        FluText {
            id : displayNameText
            text:""
            Layout.preferredWidth: 120
            wrapMode: Text.WrapAnywhere
            horizontalAlignment: Text.AlignHCenter

        }

        FluSpinBox{
            id:valueSpinBox
            Layout.preferredWidth: 120
            onDisplayTextChanged: {
                var text_value = Number(displayText);
                paramValue = text_value;
                ParamManager.setParamValue(paramIDKey, paramValue);
            }
        }


        FluIconButton{
            color: FluTheme.primaryColor.dark
            iconColor: FluColors.White
            iconSource:FluentIcons.Upload

            onClicked:{
                if(!BleUart.isConnected())
                {
                    showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                    return;
                }

                var result = Command.getParam(paramIDKey);
                switch(result)
                {
                case -1:
                    showError(paramName + qsTr(" Read Comm Error"));
                    break;
                case 0:
                    showSuccess(paramName + qsTr(" Read Success"));
                    break;
                case 1:
                    showError(paramName + qsTr(" ESC Not Support"));
                    break;
                case 2:
                    showError(paramName + qsTr(" Data Type Error"));
                    break;
                case 4:
                    showError(qsTr("Esc Config Locked"));
                    break;
                default:
                    showError(paramName + qsTr(" Data Type Error"));
                    break;
                }
            }
        }
        FluIconButton{
            color: FluTheme.primaryColor.dark
            iconColor: FluColors.White
            iconSource:FluentIcons.Download

            enabled: rw_attr === "rw"
            onClicked:{
                if(!BleUart.isConnected())
                {
                    showMessageDialog(qsTr("Error"), qsTr("Please Connect Bluetooth Device First!"));
                    return;
                }

                var result = Command.setParam(paramIDKey);
                switch(result)
                {
                case -1:
                    showError(paramName + qsTr(" Write Comm Error"));
                    break;
                case 0:
                    showSuccess(paramName + qsTr(" Write Success"));
                    break;
                case 1:
                    showError(paramName + qsTr(" ESC Not Support"));
                    break;
                case 2:
                    showError(paramName + qsTr(" Data Type Error"));
                    break;
                case 3:
                    showError(paramName + qsTr(" Data Out of Range"));
                    break;
                case 4:
                    showError(qsTr("Esc Config Locked"));
                    break;
                case 5:
                    showError(paramName + qsTr(" Need To Stop Motor First"));
                    break;
                case 6:
                    showError(paramName + qsTr(" Readonly"));
                    break;
                case 7:
                    showWarning(paramName + qsTr(" Need To Restart to Take Effect"));
                    break;
                default:
                    showError(paramName + qsTr(" Data Type Error"));
                    break;
                }
            }
        }
    }

    Connections {
        target: ParamManager
        onParamChanged: {
            if(param_id == paramIDKey)
            {
                var param_info = ParamManager.getParam(param_id);
                paramValue = param_info['cur_value'];
                valueSpinBox.displayText = paramValue.toString();
            }
        }
    }
}
