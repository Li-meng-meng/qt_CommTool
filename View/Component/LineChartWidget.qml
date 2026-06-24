import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphs 6.7
import "../Theme" as Theme

Rectangle {
    id: root

    property var timeValues: []
    property var series1Values: []
    property var series2Values: []
    property var series3Values: []

    property string chartTitle: qsTr("实时曲线")
    property string xAxisLabel: qsTr("时间 (s)")
    property string yAxisLabel: qsTr("值")
    property real yMin: -20
    property real yMax: 20
    property int maxPoints: 5000

    property color series1Color: "#EF4444"
    property color series2Color: "#22C55E"
    property color series3Color: "#3B82F6"

    property string series1Label: "Series1"
    property string series2Label: "Series2"
    property string series3Label: "Series3"

    property bool isPaused: false
    property string errorMessage: ""
    property bool hasData: timeValues.length > 0

    property bool showSeries1: true
    property bool showSeries2: true
    property bool showSeries3: true

    property bool localPaused: false

    color: Theme.ColorTheme.bgMain

    signal clearRequested()
    signal pauseToggled(bool paused)
    signal exportRequested()

    Row {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 8
        anchors.topMargin: 8
        z: 10
        spacing: 40

        Text {
            text: root.chartTitle
            color: Theme.ColorTheme.textMain
            font.family: "Roboto"
            font.pointSize: 14
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }

        Row {
            id: legendRow
            spacing: 20
            anchors.verticalCenter: parent.verticalCenter

            Rectangle {
                width: 12
                height: 3
                anchors.verticalCenter: parent.verticalCenter
                color: root.showSeries1 ? root.series1Color : Theme.ColorTheme.textDisabled
                radius: 1.5
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showSeries1 = !root.showSeries1
                }
            }
            Text {
                text: root.series1Label
                color: root.showSeries1 ? Theme.ColorTheme.textMain : Theme.ColorTheme.textDisabled
                font.family: "Roboto"
                font.pointSize: 11
                anchors.verticalCenter: parent.verticalCenter
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showSeries1 = !root.showSeries1
                }
            }

            Rectangle {
                width: 12
                height: 3
                anchors.verticalCenter: parent.verticalCenter
                color: root.showSeries2 ? root.series2Color : Theme.ColorTheme.textDisabled
                radius: 1.5
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showSeries2 = !root.showSeries2
                }
            }
            Text {
                text: root.series2Label
                color: root.showSeries2 ? Theme.ColorTheme.textMain : Theme.ColorTheme.textDisabled
                font.family: "Roboto"
                font.pointSize: 11
                anchors.verticalCenter: parent.verticalCenter
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showSeries2 = !root.showSeries2
                }
            }

            Rectangle {
                width: 12
                height: 3
                anchors.verticalCenter: parent.verticalCenter
                color: root.showSeries3 ? root.series3Color : Theme.ColorTheme.textDisabled
                radius: 1.5
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showSeries3 = !root.showSeries3
                }
            }
            Text {
                text: root.series3Label
                color: root.showSeries3 ? Theme.ColorTheme.textMain : Theme.ColorTheme.textDisabled
                font.family: "Roboto"
                font.pointSize: 11
                anchors.verticalCenter: parent.verticalCenter
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.showSeries3 = !root.showSeries3
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        anchors.topMargin: 48
        anchors.bottomMargin: 8
        anchors.leftMargin: 8
        anchors.rightMargin: 8

        GraphsView {
            id: graphView
            anchors.fill: parent

            theme: GraphsTheme {
                colorScheme: GraphsTheme.Dark
            }

            axisX: ValueAxis {
                id: xAxis
                min: 0
                max: 30
                titleText: root.xAxisLabel
            }

            axisY: ValueAxis {
                id: yAxis
                min: root.yMin
                max: root.yMax
                titleText: root.yAxisLabel
            }

            LineSeries {
                id: series1
                name: root.series1Label
                color: root.series1Color
                visible: root.showSeries1
            }

            LineSeries {
                id: series2
                name: root.series2Label
                color: root.series2Color
                visible: root.showSeries2
            }

            LineSeries {
                id: series3
                name: root.series3Label
                color: root.series3Color
                visible: root.showSeries3
            }

            function updateData() {
                if (root.localPaused) {
                    return
                }

                var count = Math.min(root.timeValues.length, root.maxPoints)
                if (count === 0) {
                    return
                }

                var lastTime = root.timeValues[root.timeValues.length - 1]
                var timeMin = Math.max(0, lastTime - 30)
                var timeMax = lastTime + 1

                xAxis.min = timeMin
                xAxis.max = timeMax
                yAxis.min = root.yMin
                yAxis.max = root.yMax

                var startIndex = Math.max(0, root.timeValues.length - count)

                if (root.showSeries1) {
                    series1.clear()
                    for (var i = startIndex; i < root.timeValues.length; i++) {
                        var t = root.timeValues[i]
                        if (t >= timeMin && t <= timeMax && i < root.series1Values.length) {
                            series1.append(t, root.series1Values[i])
                        }
                    }
                } else {
                    series1.clear()
                }

                if (root.showSeries2) {
                    series2.clear()
                    for (var i = startIndex; i < root.timeValues.length; i++) {
                        var t = root.timeValues[i]
                        if (t >= timeMin && t <= timeMax && i < root.series2Values.length) {
                            series2.append(t, root.series2Values[i])
                        }
                    }
                } else {
                    series2.clear()
                }

                if (root.showSeries3) {
                    series3.clear()
                    for (var i = startIndex; i < root.timeValues.length; i++) {
                        var t = root.timeValues[i]
                        if (t >= timeMin && t <= timeMax && i < root.series3Values.length) {
                            series3.append(t, root.series3Values[i])
                        }
                    }
                } else {
                    series3.clear()
                }
            }
        }
    }

    ChartStatusOverlay {
        anchors.fill: parent
        z: 1
        hasError: root.errorMessage !== ""
        errorMessage: root.errorMessage
        hasData: root.hasData
        onRetryClicked: {
            root.errorMessage = ""
        }
    }

    RowLayout {
        id: toolbar
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 8
        z: 2
        spacing: 8

        Button {
            id: pauseBtn
            text: root.localPaused ? qsTr("▶ 继续") : qsTr("⏸ 暂停")
            Layout.minimumWidth: 80
            onClicked: {
                root.localPaused = !root.localPaused
            }
            contentItem: Text {
                text: parent.text
                color: Theme.ColorTheme.btnNormalText
                font.family: "Roboto"
                font.pointSize: 11
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

        Button {
            id: clearBtn
            text: qsTr("🗑 清除")
            Layout.minimumWidth: 70
            onClicked: {
                series1.clear()
                series2.clear()
                series3.clear()
                graphView.lastUpdateIndex = -1
            }
            contentItem: Text {
                text: parent.text
                color: Theme.ColorTheme.btnNormalText
                font.family: "Roboto"
                font.pointSize: 11
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

        Button {
            id: exportBtn
            text: qsTr("📥 导出")
            Layout.minimumWidth: 70
            enabled: root.hasData
            onClicked: root.exportRequested()
            contentItem: Text {
                text: parent.text
                color: parent.enabled ? Theme.ColorTheme.btnNormalText : Theme.ColorTheme.textDisabled
                font.family: "Roboto"
                font.pointSize: 11
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

    function updateChart() {
        if (graphView) {
            graphView.updateData()
        }
    }

    onTimeValuesChanged: updateChart()
    onSeries1ValuesChanged: updateChart()
    onSeries2ValuesChanged: updateChart()
    onSeries3ValuesChanged: updateChart()
    onShowSeries1Changed: {
        if (!root.showSeries1) {
            series1.clear()
        }
        updateChart()
    }
    onShowSeries2Changed: {
        if (!root.showSeries2) {
            series2.clear()
        }
        updateChart()
    }
    onShowSeries3Changed: {
        if (!root.showSeries3) {
            series3.clear()
        }
        updateChart()
    }
    onYMinChanged: updateChart()
    onYMaxChanged: updateChart()
}
