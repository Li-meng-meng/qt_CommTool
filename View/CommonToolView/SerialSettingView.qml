import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme" as Theme

Rectangle {
    id: root
    width: parent.width
    height: parent.height
    color: Theme.ColorTheme.bgMain

    property real globalMargin: 16
    property real layoutSpacing: 12
    property real gridCellSpacing: 10
    property real labelMinWidth: 72

    ColumnLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: globalMargin
        spacing: layoutSpacing

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            columnSpacing: gridCellSpacing
            rowSpacing: gridCellSpacing

            Label {
                text: qsTr("端口:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            ComboBox {
                id: portCombo
                Layout.fillWidth: true
                model: root.portList
                currentIndex: root.portList.length > 0 ? 0 : -1
                onActivated: root.portName = root.portList[currentIndex]
                background: Rectangle { color: Theme.ColorTheme.bgInner; border.width: 1; border.color: Theme.ColorTheme.border; radius: 4 }
                contentItem: Text {
                    text: portCombo.currentText
                    color: Theme.ColorTheme.textMain
                    font.family: "Roboto"
                    font.pointSize: 12
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0 && currentIndex < root.portList.length) {
                        root.portName = root.portList[currentIndex]
                    }
                }
            }
            Button {
                id: refreshBtn
                Layout.fillWidth: true
                text: qsTr("刷新")
                contentItem: Text {
                    text: refreshBtn.text
                    color: Theme.ColorTheme.btnPrimaryText
                    font.family: "Roboto"
                    font.pointSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { 
                    color: refreshBtn.pressed ? Theme.ColorTheme.btnPrimaryPressed : Theme.ColorTheme.btnPrimary
                    radius: 4 
                }
                onClicked: root.refreshPorts()
            }

            Label {
                text: qsTr("波特率:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            ComboBox {
                id: baudCombo
                Layout.fillWidth: true
                Layout.columnSpan: 2
                model: [9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
                currentIndex: 7
                onActivated: root.baudRate = model[currentIndex]
                background: Rectangle { color: Theme.ColorTheme.bgInner; border.width: 1; border.color: Theme.ColorTheme.border; radius: 4 }
                contentItem: Text {
                    text: baudCombo.currentText
                    color: Theme.ColorTheme.textMain
                    font.family: "Roboto"
                    font.pointSize: 12
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0) {
                        root.baudRate = model[currentIndex]
                    }
                }
                Component.onCompleted: {
                    if (currentIndex >= 0) {
                        root.baudRate = model[currentIndex]
                    }
                }
            }

            Label {
                text: qsTr("数据位:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            ComboBox {
                id: dataBitsCombo
                Layout.fillWidth: true
                Layout.columnSpan: 2
                model: [5, 6, 7, 8]
                currentIndex: 3
                onActivated: root.dataBits = model[currentIndex]
                background: Rectangle { color: Theme.ColorTheme.bgInner; border.width: 1; border.color: Theme.ColorTheme.border; radius: 4 }
                contentItem: Text {
                    text: dataBitsCombo.currentText
                    color: Theme.ColorTheme.textMain
                    font.family: "Roboto"
                    font.pointSize: 12
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0) {
                        root.dataBits = model[currentIndex]
                    }
                }
            }

            Label {
                text: qsTr("校验位:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            ComboBox {
                id: parityCombo
                Layout.fillWidth: true
                Layout.columnSpan: 2
                model: ["None", "Odd", "Even", "Mark", "Space"]
                currentIndex: 0
                onActivated: root.parity = currentIndex
                background: Rectangle { color: Theme.ColorTheme.bgInner; border.width: 1; border.color: Theme.ColorTheme.border; radius: 4 }
                contentItem: Text {
                    text: parityCombo.currentText
                    color: Theme.ColorTheme.textMain
                    font.family: "Roboto"
                    font.pointSize: 12
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0) {
                        root.parity = currentIndex
                    }
                }
            }

            Label {
                text: qsTr("停止位:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            ComboBox {
                id: stopBitsCombo
                Layout.fillWidth: true
                Layout.columnSpan: 2
                model: ["1", "1.5", "2"]
                currentIndex: 0
                onActivated: root.stopBits = currentIndex
                background: Rectangle { color: Theme.ColorTheme.bgInner; border.width: 1; border.color: Theme.ColorTheme.border; radius: 4 }
                contentItem: Text {
                    text: stopBitsCombo.currentText
                    color: Theme.ColorTheme.textMain
                    font.family: "Roboto"
                    font.pointSize: 12
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0) {
                        root.stopBits = currentIndex
                    }
                }
            }

            Label {
                text: qsTr("流控:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            ComboBox {
                id: flowCtrlCombo
                Layout.fillWidth: true
                Layout.columnSpan: 2
                model: ["None", "RTS/CTS", "XON/XOFF"]
                currentIndex: 0
                onActivated: root.flowControl = currentIndex
                background: Rectangle { color: Theme.ColorTheme.bgInner; border.width: 1; border.color: Theme.ColorTheme.border; radius: 4 }
                contentItem: Text {
                    text: flowCtrlCombo.currentText
                    color: Theme.ColorTheme.textMain
                    font.family: "Roboto"
                    font.pointSize: 12
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0) {
                        root.flowControl = currentIndex
                    }
                }
            }

            Item {
                Layout.columnSpan: 2
            }
            Button {
                id: openCloseBtn
                Layout.fillWidth: true
                text: root.connectState === 2 ? qsTr("关闭") : qsTr("打开")
                contentItem: Text {
                    text: openCloseBtn.text
                    color: root.connectState === 2 ? Theme.ColorTheme.btnDangerText : Theme.ColorTheme.btnSuccessText
                    font.family: "Roboto"
                    font.pointSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { 
                    color: openCloseBtn.pressed 
                        ? (root.connectState === 2 ? Theme.ColorTheme.btnDangerPressed : Theme.ColorTheme.btnSuccessPressed)
                        : (root.connectState === 2 ? Theme.ColorTheme.btnDanger : Theme.ColorTheme.btnSuccess)
                    radius: 4 
                }
                onClicked: {
                    if (root.connectState === 2) {
                        root.closePort()
                    } else {
                        root.openPort(root.portName, root.baudRate, root.dataBits, root.parity, root.stopBits, root.flowControl)
                    }
                }
            }
        }
    }

    property string portName: ""
    property int baudRate: 9600
    property var portList: []
    property int connectState: 0
    property int dataBits: 8
    property int parity: 0
    property int stopBits: 0
    property int flowControl: 0

    signal openPort(string portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl)
    signal closePort()
    signal refreshPorts()
}