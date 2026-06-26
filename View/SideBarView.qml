import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./Theme" as Theme
import "./Component"

Rectangle {
    id: root
    width: parent.width
    height: parent.height
    color: Theme.ColorTheme.bgInner
    border.width: 1
    border.color: Theme.ColorTheme.border

    property real globalMargin: 16
    property real layoutSpacing: 8

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: globalMargin
        spacing: layoutSpacing

        Label {
            text: qsTr("Communication")
            color: Theme.ColorTheme.textMain
            font.family: "Roboto"
            font.pointSize: 16
            font.bold: true
            Layout.topMargin: 8
        }

        Item { Layout.fillHeight: true }

        Button {
            id: serialBtn
            text: qsTr("Serial Port")
            buttonType: "primary"
            Layout.fillWidth: true
            onClicked: root.switchToSerial()
        }

        Button {
            id: bluetoothBtn
            text: qsTr("Bluetooth")
            buttonType: "normal"
            Layout.fillWidth: true
            enabled: false
        }

        Button {
            id: wifiBtn
            text: qsTr("WiFi")
            buttonType: "normal"
            Layout.fillWidth: true
            enabled: false
        }

        Button {
            id: mqttBtn
            text: qsTr("MQTT")
            buttonType: "normal"
            Layout.fillWidth: true
            enabled: false
        }

        Button {
            id: httpBtn
            text: qsTr("HTTP")
            buttonType: "normal"
            Layout.fillWidth: true
            enabled: false
        }

        Item { Layout.fillHeight: true }

        Button {
            id: commandBtn
            text: qsTr("Commands")
            buttonType: "primary"
            Layout.fillWidth: true
            onClicked: root.switchToCommand()
        }

        Button {
            id: dataPlotBtn
            text: qsTr("Data Plot")
            buttonType: "normal"
            Layout.fillWidth: true
            onClicked: root.switchToDataPlot()
        }

        Button {
            id: settingsBtn
            text: qsTr("Settings")
            buttonType: "normal"
            Layout.fillWidth: true
            onClicked: root.openSettings()
        }
    }

    signal switchToSerial()
    signal switchToDataPlot()
    signal switchToCommand()
    signal openSettings()
}
