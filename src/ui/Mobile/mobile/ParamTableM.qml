import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.qmlmodels 1.0
import FluentUI 1.0

FluArea {
    id:paramTable
    clip: true

    property string groupDisplayName: ""
    property string groupName: ""


    Component.onCompleted: {
        refreshParamItem();

        paramListView.height = paramListModel.count*55;
        paramTable.implicitHeight = titleText.height + 10 + paramListView.height
    }

    ListModel {
        id: paramListModel
    }

    ColumnLayout {
        id:paramRect
        anchors.fill: parent

        FluText{
            id: titleText
            text:groupDisplayName

            font : FluTextStyle.Subtitle
            color: FluTheme.primaryColor.dark
        }

        ListView{
            id: paramListView
            clip: true
            spacing: 5

            Layout.fillWidth: true
            Layout.preferredHeight : paramListModel.count*55

            width: contentItem.childrenRect.width
            height:contentItem.childrenRect.height

            Component {
                id: paramIntDelegate
                ParamIntItemM{
                    paramID:param_id
                }
            }

            Component {
                id: paramFloatDelegate
                ParamFloatItemM{
                    paramID:param_id
                }
            }

            Component {
                id: paramEnumDelegate
                ParamEnumItemM{
                    paramID:param_id
                }
            }

            Component {
                id: paramStringDelegate
                ParamStringItemM{
                    paramID:param_id
                }
            }

            Component {
                id: paramIntArrayDelegate
                ParamIntArrayItemM{
                    paramID:param_id
                }
            }

            DelegateChooser
            {
                id: chooser
                role: "type"
                DelegateChoice { roleValue: "int";delegate : paramIntDelegate}
                DelegateChoice { roleValue: "float";delegate : paramFloatDelegate}
                DelegateChoice { roleValue: "enum";delegate : paramEnumDelegate}
                DelegateChoice { roleValue: "string";delegate : paramStringDelegate}
                DelegateChoice { roleValue: "int_array";delegate : paramIntArrayDelegate}
            }

            model: paramListModel
            delegate: chooser
            interactive: false
            snapMode: ListView.SnapToItem
        }
    }

    function refreshParamItem()
    {
        var param_list = ParamManager.getGroupParams(groupName);

        paramListModel.clear();

        for (var param_key_id in param_list) {
            var param_info = param_list[param_key_id];
            //console.log(JSON.stringify(param_info))
            paramListModel.append({param_id:param_info['param_id'], type:param_info['edit_type']});
        }
    }

}
