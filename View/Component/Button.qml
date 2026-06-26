import QtQuick 2.15
import QtQuick.Controls 2.15
import "../Theme" as Theme

Button {
    id: control

    property string buttonType: "normal"
    property int fontSize: 12

    readonly property var _typeColors: ({
        "primary": {
            normal: Theme.ColorTheme.btnPrimary,
            hover: Theme.ColorTheme.btnPrimaryHover,
            pressed: Theme.ColorTheme.btnPrimaryPressed,
            text: Theme.ColorTheme.btnPrimaryText,
            border: "transparent"
        },
        "normal": {
            normal: Theme.ColorTheme.btnNormal,
            hover: Theme.ColorTheme.btnNormalHover,
            pressed: Theme.ColorTheme.btnNormalHover,
            text: Theme.ColorTheme.btnNormalText,
            border: Theme.ColorTheme.btnNormalBorder
        },
        "danger": {
            normal: Theme.ColorTheme.btnDanger,
            hover: Theme.ColorTheme.btnDangerHover,
            pressed: Theme.ColorTheme.btnDangerPressed,
            text: Theme.ColorTheme.btnDangerText,
            border: "transparent"
        },
        "success": {
            normal: Theme.ColorTheme.btnSuccess,
            hover: Theme.ColorTheme.btnSuccessHover,
            pressed: Theme.ColorTheme.btnSuccessPressed,
            text: Theme.ColorTheme.btnSuccessText,
            border: "transparent"
        }
    })

    readonly property var _colors: _typeColors[buttonType] || _typeColors["normal"]

    background: Rectangle {
        color: control.enabled ? (control.down ? _colors.pressed : (control.hovered ? _colors.hover : _colors.normal)) : Theme.ColorTheme.bgMain
        border.width: buttonType === "normal" ? 1 : 0
        border.color: control.enabled ? _colors.border : Theme.ColorTheme.border
        radius: 4
    }

    contentItem: Text {
        text: control.text
        color: control.enabled ? _colors.text : Theme.ColorTheme.textSub
        font.family: "Roboto"
        font.pixelSize: fontSize
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
