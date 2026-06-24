import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./Theme" as Theme
import "./CommonToolView"

Rectangle {
    id: root
    width: parent.width
    height: parent.height
    color: Theme.ColorTheme.bgMain

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 8

            Rectangle {
            width: 12
            height: 12
            radius: 6
            color: root.connectState === 2 ? Theme.ColorTheme.textSuccess : Theme.ColorTheme.textDisabled
        }

        Text {
            text: root.connectState === 2 ? qsTr("已连接") : qsTr("未连接")
            color: root.connectState === 2 ? Theme.ColorTheme.textSuccess : Theme.ColorTheme.textDisabled
            font.family: "Roboto"
            font.pointSize: 12
        }
        }

        SerialSettingView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            portName: root.portName
            baudRate: root.baudRate
            portList: root.portList
            connectState: root.connectState
            dataBits: root.dataBits
            parity: root.parity
            stopBits: root.stopBits
            flowControl: root.flowControl

            onOpenPort: root.openPort(portName, baudRate, dataBits, parity, stopBits, flowControl)
            onClosePort: root.closePort()
            onRefreshPorts: root.refreshPorts()

            onPortNameChanged: root.portName = portName
            onBaudRateChanged: root.baudRate = baudRate
            onDataBitsChanged: root.dataBits = dataBits
            onParityChanged: root.parity = parity
            onStopBitsChanged: root.stopBits = stopBits
            onFlowControlChanged: root.flowControl = flowControl
        }
    }

    property string portName: ""
    property int baudRate: 0
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