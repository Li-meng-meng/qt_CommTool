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

        // 设备列表
        GridLayout {
            Layout.fillWidth: true
            columns: 3
            columnSpacing: gridCellSpacing
            rowSpacing: gridCellSpacing

            Label {
                text: qsTr("设备:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            ComboBox {
                id: deviceCombo
                Layout.fillWidth: true
                model: root.deviceList
                currentIndex: root.deviceList.length > 0 ? 0 : -1
                onActivated: {
                    if (currentIndex >= 0 && currentIndex < root.deviceList.length) {
                        root.selectedDevice = root.deviceList[currentIndex]
                    }
                }
                background: Rectangle { color: Theme.ColorTheme.bgInner; border.width: 1; border.color: Theme.ColorTheme.border; radius: 4 }
                contentItem: Text {
                    text: deviceCombo.currentText
                    color: Theme.ColorTheme.textMain
                    font.family: "Roboto"
                    font.pointSize: 12
                    elide: Text.ElideRight
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0 && currentIndex < root.deviceList.length) {
                        root.selectedDevice = root.deviceList[currentIndex]
                    }
                }
            }
            Button {
                id: scanBtn
                Layout.fillWidth: true
                text: root.isScanning ? qsTr("停止") : qsTr("扫描")
                contentItem: Text {
                    text: scanBtn.text
                    color: root.isScanning ? Theme.ColorTheme.btnDangerText : Theme.ColorTheme.btnPrimaryText
                    font.family: "Roboto"
                    font.pointSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: scanBtn.pressed
                        ? (root.isScanning ? Theme.ColorTheme.btnDangerPressed : Theme.ColorTheme.btnPrimaryPressed)
                        : (root.isScanning ? Theme.ColorTheme.btnDanger : Theme.ColorTheme.btnPrimary)
                    radius: 4
                }
                onClicked: {
                    console.log("[BluetoothSettingView] scanBtn clicked, isScanning:", root.isScanning)
                    if (root.isScanning) {
                        root.stopScan()
                    } else {
                        root.startScan()
                    }
                }
            }

            // Service UUID
            Label {
                text: qsTr("Service UUID:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            TextField {
                id: serviceUuidField
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: root.serviceUuid
                placeholderText: qsTr("输入 Service UUID")
                font.family: "Roboto"
                font.pointSize: 12
                color: Theme.ColorTheme.textMain
                onEditingFinished: root.serviceUuid = text
                background: Rectangle {
                    color: Theme.ColorTheme.bgInner
                    border.width: serviceUuidField.activeFocus ? 2 : 1
                    border.color: serviceUuidField.activeFocus ? Theme.ColorTheme.btnPrimary : Theme.ColorTheme.border
                    radius: 4
                }
            }

            // Write UUID
            Label {
                text: qsTr("Write UUID:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            TextField {
                id: writeCharUuidField
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: root.writeCharUuid
                placeholderText: qsTr("输入 Write Characteristic UUID")
                font.family: "Roboto"
                font.pointSize: 12
                color: Theme.ColorTheme.textMain
                onEditingFinished: root.writeCharUuid = text
                background: Rectangle {
                    color: Theme.ColorTheme.bgInner
                    border.width: writeCharUuidField.activeFocus ? 2 : 1
                    border.color: writeCharUuidField.activeFocus ? Theme.ColorTheme.btnPrimary : Theme.ColorTheme.border
                    radius: 4
                }
            }

            // Notify UUID
            Label {
                text: qsTr("Notify UUID:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            TextField {
                id: notifyCharUuidField
                Layout.fillWidth: true
                Layout.columnSpan: 2
                text: root.notifyCharUuid
                placeholderText: qsTr("输入 Notify Characteristic UUID")
                font.family: "Roboto"
                font.pointSize: 12
                color: Theme.ColorTheme.textMain
                onEditingFinished: root.notifyCharUuid = text
                background: Rectangle {
                    color: Theme.ColorTheme.bgInner
                    border.width: notifyCharUuidField.activeFocus ? 2 : 1
                    border.color: notifyCharUuidField.activeFocus ? Theme.ColorTheme.btnPrimary : Theme.ColorTheme.border
                    radius: 4
                }
            }

            // 连接状态
            Label {
                text: qsTr("状态:")
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                Layout.minimumWidth: labelMinWidth
                Layout.alignment: Qt.AlignVCenter
            }
            Label {
                id: stateLabel
                Layout.fillWidth: true
                text: root.connectState === 2 ? qsTr("已连接") :
                      root.connectState === 1 ? qsTr("连接中") : qsTr("未连接")
                color: root.connectState === 2 ? Theme.ColorTheme.btnSuccess :
                       root.connectState === 1 ? Theme.ColorTheme.btnWarning : Theme.ColorTheme.textSub
                font.family: "Roboto"
                font.pointSize: 12
            }

            // 连接/断开按钮
            Item {
                Layout.columnSpan: 2
            }
            Button {
                id: connectBtn
                Layout.fillWidth: true
                text: root.connectState === 2 ? qsTr("断开") : qsTr("连接")
                contentItem: Text {
                    text: connectBtn.text
                    color: root.connectState === 2 ? Theme.ColorTheme.btnDangerText : Theme.ColorTheme.btnSuccessText
                    font.family: "Roboto"
                    font.pointSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: connectBtn.pressed
                        ? (root.connectState === 2 ? Theme.ColorTheme.btnDangerPressed : Theme.ColorTheme.btnSuccessPressed)
                        : (root.connectState === 2 ? Theme.ColorTheme.btnDanger : Theme.ColorTheme.btnSuccess)
                    radius: 4
                }
                onClicked: {
                    console.log("[BluetoothSettingView] connectBtn clicked, connectState:", root.connectState)
                    if (root.connectState === 2) {
                        root.closePort()
                    } else {
                        // 从设备字符串中提取地址 (格式: "name (address)")
                        var deviceStr = root.selectedDevice
                        var addressMatch = deviceStr.match(/\(([^)]+)\)/)
                        var address = addressMatch ? addressMatch[1] : deviceStr
                        console.log("[BluetoothSettingView] Connecting to address:", address)
                        root.openPort(address, root.serviceUuid, root.writeCharUuid, root.notifyCharUuid)
                    }
                }
            }
        }
    }

    property var deviceList: []
    property string selectedDevice: ""
    property bool isScanning: false
    property int connectState: 0
    property string serviceUuid: "0000ffe0-0000-1000-8000-00805f9b34fb"
    property string writeCharUuid: "0000ffe1-0000-1000-8000-00805f9b34fb"
    property string notifyCharUuid: "0000ffe1-0000-1000-8000-00805f9b34fb"

    signal startScan()
    signal stopScan()
    signal openPort(string deviceAddress, string serviceUuid, string writeCharUuid, string notifyCharUuid)
    signal closePort()
}