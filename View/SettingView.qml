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
            id: serialSetting
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: root.currentCommType === "serial"

            portList: root.portList
            connectState: root.connectState

            onOpenPort: root.openPort(portName, baudRate, dataBits, parity, stopBits, flowControl)
            onClosePort: root.closePort()
            onRefreshPorts: root.refreshPorts()
        }

        BluetoothSettingView {
            id: bluetoothSetting
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: root.currentCommType === "bluetooth"

            deviceList: root.bluetoothDeviceList
            isScanning: root.isScanning
            connectState: root.connectState

            onStartScan: root.startScan()
            onStopScan: root.stopScan()
            onOpenPort: root.openBluetoothPort(deviceAddress, serviceUuid, writeCharUuid, notifyCharUuid)
            onClosePort: root.closePort()
        }
    }

    property var portList: []
    property var bluetoothDeviceList: []
    property bool isScanning: false
    property int connectState: 0
    property string currentCommType: "serial"

    signal openPort(string portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl)
    signal openBluetoothPort(string deviceAddress, string serviceUuid, string writeCharUuid, string notifyCharUuid)
    signal closePort()
    signal refreshPorts()
    signal startScan()
    signal stopScan()
}