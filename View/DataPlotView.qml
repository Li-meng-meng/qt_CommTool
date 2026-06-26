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
    property var commandViewModel: null

    Connections {
        target: root.commandViewModel
        function onChartDataChanged() {
            for (var i = 0; i < chartRepeater.count; i++) {
                var item = chartRepeater.itemAt(i)
                if (item && item.updateChart) {
                    item.updateChart()
                }
            }
        }
    }

    Connections {
        target: root.dataPlotViewModel
        function onDataChanged() {
            for (var i = 0; i < chartRepeater.count; i++) {
                var item = chartRepeater.itemAt(i)
                if (item && item.updateChart) {
                    item.updateChart()
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Repeater {
            id: chartRepeater
            model: root.commandViewModel && root.commandViewModel.chartConfigs.length > 0
                ? root.commandViewModel.chartConfigs
                : defaultChartConfigs

            property var defaultChartConfigs: [
                {
                    title: qsTr("加速度实时曲线"),
                    yAxisLabel: qsTr("加速度 (m/s²)"),
                    yMin: -20,
                    yMax: 20,
                    seriesLabels: ["AX", "AY", "AZ"]
                },
                {
                    title: qsTr("角速度实时曲线"),
                    yAxisLabel: qsTr("角速度 (°/s)"),
                    yMin: -500,
                    yMax: 500,
                    seriesLabels: ["GX", "GY", "GZ"]
                },
                {
                    title: qsTr("姿态角实时曲线"),
                    yAxisLabel: qsTr("角度 (°)"),
                    yMin: -180,
                    yMax: 180,
                    seriesLabels: ["Roll", "Pitch", "Yaw"]
                },
                {
                    title: qsTr("磁场实时曲线"),
                    yAxisLabel: qsTr("磁场 (uT)"),
                    yMin: -100,
                    yMax: 100,
                    seriesLabels: ["MX", "MY", "MZ"]
                }
            ]

            delegate: LineChartWidget {
                Layout.fillWidth: true
                Layout.fillHeight: !collapsed
                Layout.preferredHeight: collapsed ? 36 : -1
                Layout.minimumHeight: collapsed ? 36 : 0
                title: modelData.title
                yAxisLabel: modelData.yAxisLabel
                yMin: modelData.yMin
                yMax: modelData.yMax
                series1Label: modelData.seriesLabels ? modelData.seriesLabels[0] : "S1"
                series2Label: modelData.seriesLabels && modelData.seriesLabels.length > 1 ? modelData.seriesLabels[1] : "S2"
                series3Label: modelData.seriesLabels && modelData.seriesLabels.length > 2 ? modelData.seriesLabels[2] : "S3"
                showSeries2: !modelData.seriesLabels || modelData.seriesLabels.length > 1
                showSeries3: !modelData.seriesLabels || modelData.seriesLabels.length > 2
                maxPoints: 5000
                localPaused: root.commandViewModel && root.commandViewModel.chartConfigs.length > 0
                    ? root.commandViewModel.chartPaused
                    : (root.dataPlotViewModel ? root.dataPlotViewModel.isPaused : false)

                property int _chartDataVer: root.commandViewModel ? root.commandViewModel.chartDataVersion : 0

                timeValues: {
                    var ver = _chartDataVer
                    if (root.commandViewModel && root.commandViewModel.chartConfigs.length > 0) {
                        return root.commandViewModel.getChartTimeValues(index)
                    } else if (root.dataPlotViewModel) {
                        return root.dataPlotViewModel.timeValues
                    } else {
                        return []
                    }
                }
                series1Values: {
                    var ver = _chartDataVer
                    if (root.commandViewModel && root.commandViewModel.chartConfigs.length > 0) {
                        return root.commandViewModel.getChartSeriesValues(index, 0)
                    } else if (root.dataPlotViewModel) {
                        if (index === 0) return root.dataPlotViewModel.xValues
                        else if (index === 1) return root.dataPlotViewModel.gxValues
                        else if (index === 2) return root.dataPlotViewModel.rollValues
                        else return root.dataPlotViewModel.mxValues
                    } else {
                        return []
                    }
                }
                series2Values: {
                    var ver = _chartDataVer
                    if (root.commandViewModel && root.commandViewModel.chartConfigs.length > 0) {
                        return root.commandViewModel.getChartSeriesValues(index, 1)
                    } else if (root.dataPlotViewModel) {
                        if (index === 0) return root.dataPlotViewModel.yValues
                        else if (index === 1) return root.dataPlotViewModel.gyValues
                        else if (index === 2) return root.dataPlotViewModel.pitchValues
                        else return root.dataPlotViewModel.myValues
                    } else {
                        return []
                    }
                }
                series3Values: {
                    var ver = _chartDataVer
                    if (root.commandViewModel && root.commandViewModel.chartConfigs.length > 0) {
                        return root.commandViewModel.getChartSeriesValues(index, 2)
                    } else if (root.dataPlotViewModel) {
                        if (index === 0) return root.dataPlotViewModel.zValues
                        else if (index === 1) return root.dataPlotViewModel.gzValues
                        else if (index === 2) return root.dataPlotViewModel.yawValues
                        else return root.dataPlotViewModel.mzValues
                    } else {
                        return []
                    }
                }

                onClearData: {
                    if (root.commandViewModel && root.commandViewModel.chartConfigs.length > 0) {
                        root.commandViewModel.clearChartData()
                    } else if (root.dataPlotViewModel) {
                        root.dataPlotViewModel.clearChart()
                    }
                }

                onExportData: {
                    exportPopup.visible = true
                }
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
                        text: "chart_data.csv"
                        placeholderText: qsTr("输入文件名")
                    }

                    Button {
                        text: qsTr("导出")
                        onClicked: {
                            var fileName = exportPath.text || "chart_data.csv"
                            if (!fileName.endsWith(".csv")) {
                                fileName += ".csv"
                            }
                            exportPopup.visible = false
                            toast.show(qsTr("导出功能开发中"), "info")
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
