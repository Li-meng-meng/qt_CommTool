import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: control

    property color normalColor: "#3498db"
    property color hoverColor: "#2980b9"
    property color pressedColor: "#1f618d"

    background: Rectangle {
        color: control.down ? pressedColor : (control.hovered ? hoverColor : normalColor)
        radius: 4
    }

    contentItem: Text {
        text: control.text
        color: "#ffffff"
        font.pixelSize: 14
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}