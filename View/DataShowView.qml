import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import "./Theme" as Theme

Rectangle {
    id: root
    width: parent.width
    height: parent.height
    color: Theme.ColorTheme.bgMain

    property string recvData: ""
    property bool hexDisplay: true
    property bool hexSend: true

    property real globalMargin: 16
    property real layoutSpacing: 12

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: globalMargin
        spacing: layoutSpacing

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Receive Data:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                font.pointSize: 12
                font.bold: true
            }

            CheckBox {
                id: hexDisplayCheck
                text: qsTr("Hex")
                checked: root.hexDisplay
                onCheckedChanged: root.hexDisplay = checked
            }

            Button {
                text: qsTr("Clear")
                onClicked: root.clearReceive()
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

        Controls.ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            TextArea {
                id: recvArea
                text: root.recvData
                readOnly: true
                font.family: "Consolas"
                font.pointSize: 12
                color: Theme.ColorTheme.textMain
                wrapMode: TextArea.NoWrap
                selectByMouse: true
                background: Rectangle {
                    color: Theme.ColorTheme.bgInner
                    border.width: 1
                    border.color: Theme.ColorTheme.border
                    radius: 4
                }
                onTextChanged: {
                    // 自动滚动到底部
                    scrollView.ScrollBar.vertical.position = 1.0
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Send Data:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                font.pointSize: 12
                font.bold: true
            }

            CheckBox {
                id: hexSendCheck
                text: qsTr("Hex")
                checked: root.hexSend
                onCheckedChanged: root.hexSend = checked
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: layoutSpacing

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                color: Theme.ColorTheme.bgInner
                border.width: 1
                border.color: Theme.ColorTheme.border
                radius: 4

                TextInput {
                    id: sendInput
                    anchors.fill: parent
                    anchors.margins: 4
                    font.family: "Consolas"
                    font.pointSize: 12
                    color: Theme.ColorTheme.textMain
                }
            }

            Button {
                id: sendBtn
                text: qsTr("Send")
                Layout.minimumWidth: 80
                onClicked: {
                    if (sendInput.text.length > 0) {
                        root.sendData(sendInput.text)
                        sendInput.text = ""
                    }
                }
                contentItem: Text {
                    text: parent.text
                    color: Theme.ColorTheme.btnPrimaryText
                    font.family: "Roboto"
                    font.pointSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: parent.pressed ? Theme.ColorTheme.btnPrimaryHover : Theme.ColorTheme.btnPrimary
                    radius: 4
                }
            }
        }
    }

    signal sendData(string data)
    signal clearReceive()
}