import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtGraphs
import "../Theme" as Theme

Rectangle {
    id: root
    color: Theme.ColorTheme.bgChart
    radius: 8

    property var timeValues: []
    property var series1Values: []
    property var series2Values: []
    property var series3Values: []
    property string title: "Chart"
    property string series1Label: "X"
    property string series2Label: "Y"
    property string series3Label: "Z"
    property string yAxisLabel: "Value"
    property double yMin: -10
    property double yMax: 10
    property int maxPoints: 5000
    property bool showSeries1: true
    property bool showSeries2: true
    property bool showSeries3: true
    property bool localPaused: false

    property int lastUpdateIndex: -1

    signal clearData()
    signal exportData()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 2
        spacing: 2

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Row {
                spacing: 8
                Text {
                    text: root.title
                    color: Theme.ColorTheme.textMain
                    font.bold: true
                    font.pixelSize: 11
                }

                Row {
                    spacing: 6

                    Row {
                        spacing: 3
                        MouseArea {
                            width: 10
                            height: 10
                            Rectangle {
                                anchors.fill: parent
                                color: "#3B82F6"
                                radius: 2
                            }
                            onClicked: root.showSeries1 = !root.showSeries1
                        }
                        Text {
                            text: root.series1Label
                            color: root.showSeries1 ? Theme.ColorTheme.textMain : Theme.ColorTheme.textSub
                            font.pixelSize: 10
                        }
                    }

                    Row {
                        spacing: 3
                        MouseArea {
                            width: 10
                            height: 10
                            Rectangle {
                                anchors.fill: parent
                                color: "#EF4444"
                                radius: 2
                            }
                            onClicked: root.showSeries2 = !root.showSeries2
                        }
                        Text {
                            text: root.series2Label
                            color: root.showSeries2 ? Theme.ColorTheme.textMain : Theme.ColorTheme.textSub
                            font.pixelSize: 10
                        }
                    }

                    Row {
                        spacing: 3
                        MouseArea {
                            width: 10
                            height: 10
                            Rectangle {
                                anchors.fill: parent
                                color: "#10B981"
                                radius: 2
                            }
                            onClicked: root.showSeries3 = !root.showSeries3
                        }
                        Text {
                            text: root.series3Label
                            color: root.showSeries3 ? Theme.ColorTheme.textMain : Theme.ColorTheme.textSub
                            font.pixelSize: 10
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Row {
                spacing: 3
                Button {
                    text: localPaused ? "Resume" : "Pause"
                    font.pixelSize: 10
                    onClicked: {
                        root.localPaused = !root.localPaused
                    }
                }

                Button {
                    text: "Clear"
                    font.pixelSize: 10
                    onClicked: {
                        series1.clear()
                        series2.clear()
                        series3.clear()
                        root.lastUpdateIndex = -1
                    }
                }

                Button {
                    text: "Export"
                    font.pixelSize: 10
                    onClicked: root.exportData()
                }
            }
        }

        GraphsView {
            id: graphsView
            Layout.fillWidth: true
            Layout.fillHeight: true

            axisX: ValueAxis {
                id: xAxis
                titleText: "Time (s)"
                labelFormat: "%.1f"
                gridVisible: false
            }

            axisY: ValueAxis {
                id: yAxis
                min: root.yMin
                max: root.yMax
                labelFormat: "%.1f"
                gridVisible: false
            }

            LineSeries {
                id: series1
                name: root.series1Label
                color: "#3B82F6"
                width: 1.5
                visible: root.showSeries1
            }

            LineSeries {
                id: series2
                name: root.series2Label
                color: "#EF4444"
                width: 1.5
                visible: root.showSeries2
            }

            LineSeries {
                id: series3
                name: root.series3Label
                color: "#10B981"
                width: 1.5
                visible: root.showSeries3
            }

            function updateVisibleRange() {
                var dataCount = root.timeValues.length
                if (dataCount === 0) return

                var lastTime = root.timeValues[dataCount - 1]
                var timeMin = Math.max(0, lastTime - 30)
                var timeMax = lastTime + 1

                xAxis.min = timeMin
                xAxis.max = timeMax
                yAxis.min = root.yMin
                yAxis.max = root.yMax

                return { min: timeMin, max: timeMax }
            }
        }
    }

    function updateChart() {
        if (root.localPaused) return

        var dataCount = root.timeValues.length
        if (dataCount === 0) {
            series1.clear()
            series2.clear()
            series3.clear()
            root.lastUpdateIndex = -1
            return
        }

        var range = graphsView.updateVisibleRange()
        var startIndex = 0

        if (root.lastUpdateIndex >= 0 && root.lastUpdateIndex < dataCount) {
            startIndex = root.lastUpdateIndex + 1
        }

        var series1Count = root.series1Values.length
        var series2Count = root.series2Values.length
        var series3Count = root.series3Values.length

        for (var i = startIndex; i < dataCount; i++) {
            var t = root.timeValues[i]
            if (t >= range.min && t <= range.max) {
                if (root.showSeries1 && i < series1Count) {
                    series1.append(t, root.series1Values[i])
                }
                if (root.showSeries2 && i < series2Count) {
                    series2.append(t, root.series2Values[i])
                }
                if (root.showSeries3 && i < series3Count) {
                    series3.append(t, root.series3Values[i])
                }
            }
        }

        root.lastUpdateIndex = dataCount - 1

        trimSeries(series1)
        trimSeries(series2)
        trimSeries(series3)
    }

    function trimSeries(series) {
        while (series.count > root.maxPoints) {
            series.remove(0)
        }
    }

    onShowSeries1Changed: {
        if (!root.showSeries1) {
            series1.clear()
        } else if (root.lastUpdateIndex >= 0) {
            rebuildSeries(series1, root.series1Values)
        }
    }

    onShowSeries2Changed: {
        if (!root.showSeries2) {
            series2.clear()
        } else if (root.lastUpdateIndex >= 0) {
            rebuildSeries(series2, root.series2Values)
        }
    }

    onShowSeries3Changed: {
        if (!root.showSeries3) {
            series3.clear()
        } else if (root.lastUpdateIndex >= 0) {
            rebuildSeries(series3, root.series3Values)
        }
    }

    function rebuildSeries(series, values) {
        var dataCount = root.timeValues.length
        if (dataCount === 0 || values.length === 0) return

        series.clear()

        var range = graphsView.updateVisibleRange()
        var startIndex = Math.max(0, dataCount - root.maxPoints)

        for (var i = startIndex; i < dataCount; i++) {
            var t = root.timeValues[i]
            if (t >= range.min && t <= range.max && i < values.length) {
                series.append(t, values[i])
            }
        }
    }
}