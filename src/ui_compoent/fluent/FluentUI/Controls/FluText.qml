import QtQuick 2.10
import QtQuick.Controls 1.3
import FluentUI 1.0

Text {
    property color textColor: FluTheme.dark ? FluColors.White : FluColors.Grey220
    id:text
    color: textColor
    renderType: FluTheme.nativeText ? Text.NativeRendering : Text.QtRendering
    font: FluTextStyle.Body
}
