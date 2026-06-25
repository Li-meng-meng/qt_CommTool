import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CommTool 1.0
import "./Theme" as Theme
import "./Component"

Rectangle {
    id: root
    width: parent.width
    height: parent.height
    color: Theme.ColorTheme.bgMain

    property var dataPlotViewModel: null

    Connections {
        target: root.dataPlotViewModel
        function onDataChanged() {
            accelChart.updateChart()
            gyroChart.updateChart()
            angleChart.updateChart()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        LineChartWidget {
            id: accelChart
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("加速度实时曲线")
            yAxisLabel: qsTr("加速度 (m/s²)")
            yMin: -20
            yMax: 20
            series1Label: "AX"
            series2Label: "AY"
            series3Label: "AZ"
            maxPoints: 5000
            localPaused: root.dataPlotViewModel ? root.dataPlotViewModel.isPaused : false

            timeValues: root.dataPlotViewModel ? root.dataPlotViewModel.timeValues : []
            series1Values: root.dataPlotViewModel ? root.dataPlotViewModel.xValues : []
            series2Values: root.dataPlotViewModel ? root.dataPlotViewModel.yValues : []
            series3Values: root.dataPlotViewModel ? root.dataPlotViewModel.zValues : []

            onClearData: {
                if (root.dataPlotViewModel) {
                    root.dataPlotViewModel.clearChart()
                }
            }

            onExportData: {
                exportPopup.visible = true
            }
        }

        LineChartWidget {
            id: gyroChart
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("角速度实时曲线")
            yAxisLabel: qsTr("角速度 (°/s)")
            yMin: -500
            yMax: 500
            series1Label: "GX"
            series2Label: "GY"
            series3Label: "GZ"
            maxPoints: 5000
            localPaused: root.dataPlotViewModel ? root.dataPlotViewModel.isPaused : false

            timeValues: root.dataPlotViewModel ? root.dataPlotViewModel.timeValues : []
            series1Values: root.dataPlotViewModel ? root.dataPlotViewModel.gxValues : []
            series2Values: root.dataPlotViewModel ? root.dataPlotViewModel.gyValues : []
            series3Values: root.dataPlotViewModel ? root.dataPlotViewModel.gzValues : []

            onClearData: {
                if (root.dataPlotViewModel) {
                    root.dataPlotViewModel.clearChart()
                }
            }

            onExportData: {
                exportPopup.visible = true
            }
        }

        LineChartWidget {
            id: angleChart
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("姿态角实时曲线")
            yAxisLabel: qsTr("角度 (°)")
            yMin: -180
            yMax: 180
            series1Label: "Roll"
            series2Label: "Pitch"
            series3Label: "Yaw"
            maxPoints: 5000
            localPaused: root.dataPlotViewModel ? root.dataPlotViewModel.isPaused : false

            timeValues: root.dataPlotViewModel ? root.dataPlotViewModel.timeValues : []
            series1Values: root.dataPlotViewModel ? root.dataPlotViewModel.rollValues : []
            series2Values: root.dataPlotViewModel ? root.dataPlotViewModel.pitchValues : []
            series3Values: root.dataPlotViewModel ? root.dataPlotViewModel.yawValues : []

            onClearData: {
                if (root.dataPlotViewModel) {
                    root.dataPlotViewModel.clearChart()
                }
            }

            onExportData: {
                exportPopup.visible = true
            }
        }
    }

    Popup {
        id: exportPopup
        anchors.centerIn: parent
        width: 300
        height: 120
        modal: true
        focus: true

        Rectangle {
            anchors.fill: parent
            color: Theme.ColorTheme.bgInner
            radius: 8

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Text {
                    text: qsTr("导出数据")
                    font.family: "Roboto"
                    font.pointSize: 14
                    font.bold: true
                    color: Theme.ColorTheme.textMain
                }

                RowLayout {
                    spacing: 8

                    TextField {
                        id: exportPath
                        Layout.fillWidth: true
                        text: "acceleration_data.csv"
                        placeholderText: qsTr("输入文件名")
                    }

                    Button {
                        text: qsTr("导出")
                        onClicked: {
                            var fileName = exportPath.text || "acceleration_data.csv"
                            if (!fileName.endsWith(".csv")) {
                                fileName += ".csv"
                            }
                            if (root.dataPlotViewModel) {
                                var success = root.dataPlotViewModel.exportChartData(fileName)
                                exportPopup.visible = false
                                if (success) {
                                    toast.show(qsTr("数据已导出"), "success")
                                } else {
                                    toast.show(qsTr("导出失败"), "error")
                                }
                            }
                        }
                        contentItem: Text {
                            text: parent.text
                            color: Theme.ColorTheme.btnPrimaryText
                            font.family: "Roboto"
                            font.pointSize: 12
                            horizontalAlignment: Text.AlignHCenter
                        }
                        background: Rectangle {
                            color: Theme.ColorTheme.btnPrimary
                            radius: 4
                        }
                    }
                }
            }
        }
    }

    property var toast: null
}