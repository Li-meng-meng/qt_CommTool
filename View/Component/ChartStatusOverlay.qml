import QtQuick 2.15
import QtQuick.Controls 2.15
import "../Theme" as Theme

Rectangle {
    id: root

    property bool hasError: false
    property string errorMessage: ""
    property bool hasData: true

    signal retryClicked()

    color: "transparent"

    Rectangle {
        anchors.fill: parent
        color: "#80000000"
        visible: root.hasError || !root.hasData
        z: 100
    }

    Column {
        anchors.centerIn: parent
        spacing: 16
        visible: root.hasError || !root.hasData

        Text {
            text: root.hasError ? root.errorMessage : qsTr("暂无数据")
            color: root.hasError ? "#EF4444" : Theme.ColorTheme.textSub
            font.family: "Roboto"
            font.pointSize: 14
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            text: qsTr("重试")
            visible: root.hasError
            onClicked: root.retryClicked()
            contentItem: Text {
                text: parent.text
                color: Theme.ColorTheme.btnNormalText
                font.family: "Roboto"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: parent.pressed ? Theme.ColorTheme.btnNormalHover : Theme.ColorTheme.btnNormal
                border.width: 1
                border.color: Theme.ColorTheme.btnNormalBorder
                radius: 4
            }
        }
    }
}