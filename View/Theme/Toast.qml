import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    visible: false
    opacity: 0
    radius: 8
    anchors.centerIn: parent
    width: 200
    height: 60
    z: 100

    property string message: ""
    property color bgColor: "#1E293B"
    property color textColor: "#E2E8F0"

    Text {
        text: root.message
        color: root.textColor
        font.family: "Roboto"
        font.pointSize: 14
        anchors.centerIn: parent
    }

    Timer {
        id: hideTimer
        interval: 2000
        running: false
        onTriggered: {
            visible = false
        }
    }

    function show(msg, type) {
        message = msg
        if (type === "success") {
            bgColor = "#16A34A"
            textColor = "#FFFFFF"
        } else if (type === "error") {
            bgColor = "#DC2626"
            textColor = "#FFFFFF"
        } else {
            bgColor = "#1E293B"
            textColor = "#E2E8F0"
        }
        color = bgColor
        visible = true
        opacity = 1

        hideTimer.restart()
    }
}