import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./Theme" as Theme

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
            Layout.fillWidth: true
            onClicked: root.switchToSerial()
            contentItem: Text {
                text: serialBtn.text
                color: Theme.ColorTheme.btnPrimaryText
                font.family: "Roboto"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: Theme.ColorTheme.btnPrimary
                radius: 4
            }
        }

        Button {
            id: bluetoothBtn
            text: qsTr("Bluetooth")
            Layout.fillWidth: true
            enabled: false
            contentItem: Text {
                text: bluetoothBtn.text
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: Theme.ColorTheme.bgMain
                border.width: 1
                border.color: Theme.ColorTheme.border
                radius: 4
            }
        }

        Button {
            id: wifiBtn
            text: qsTr("WiFi")
            Layout.fillWidth: true
            enabled: false
            contentItem: Text {
                text: wifiBtn.text
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: Theme.ColorTheme.bgMain
                border.width: 1
                border.color: Theme.ColorTheme.border
                radius: 4
            }
        }

        Button {
            id: mqttBtn
            text: qsTr("MQTT")
            Layout.fillWidth: true
            enabled: false
            contentItem: Text {
                text: mqttBtn.text
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: Theme.ColorTheme.bgMain
                border.width: 1
                border.color: Theme.ColorTheme.border
                radius: 4
            }
        }

        Button {
            id: httpBtn
            text: qsTr("HTTP")
            Layout.fillWidth: true
            enabled: false
            contentItem: Text {
                text: httpBtn.text
                color: Theme.ColorTheme.textSub
                font.family: "Roboto"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: Theme.ColorTheme.bgMain
                border.width: 1
                border.color: Theme.ColorTheme.border
                radius: 4
            }
        }

        Item { Layout.fillHeight: true }

        Button {
            id: dataPlotBtn
            text: qsTr("Data Plot")
            Layout.fillWidth: true
            onClicked: root.switchToDataPlot()
            contentItem: Text {
                text: dataPlotBtn.text
                color: Theme.ColorTheme.btnNormalText
                font.family: "Roboto"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: Theme.ColorTheme.btnNormal
                border.width: 1
                border.color: Theme.ColorTheme.btnNormalBorder
                radius: 4
            }
        }

        Button {
            id: settingsBtn
            text: qsTr("Settings")
            Layout.fillWidth: true
            onClicked: root.openSettings()
            contentItem: Text {
                text: settingsBtn.text
                color: Theme.ColorTheme.btnNormalText
                font.family: "Roboto"
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: Theme.ColorTheme.btnNormal
                border.width: 1
                border.color: Theme.ColorTheme.btnNormalBorder
                radius: 4
            }
        }
    }

    signal switchToSerial()
    signal switchToDataPlot()
    signal openSettings()
}