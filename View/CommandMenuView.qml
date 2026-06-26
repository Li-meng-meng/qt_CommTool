import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./Theme" as Theme

Rectangle {
    id: root
    color: Theme.ColorTheme.bgMain
    radius: 4
    property var viewModel: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Text {
            text: "协议调试工具"
            color: Theme.ColorTheme.textMain
            font.bold: true
            font.pixelSize: 16
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Rectangle {
                Layout.preferredWidth: 280
                Layout.fillHeight: true
                color: Theme.ColorTheme.bgInner
                radius: 4
                border.color: Theme.ColorTheme.border
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 6

                    Text {
                        text: "CMD命令列表"
                        color: Theme.ColorTheme.textMain
                        font.bold: true
                        font.pixelSize: 12
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: searchField
                        Layout.fillWidth: true
                        placeholderText: "搜索命令..."
                        font.pixelSize: 12
                        background: Rectangle {
                            color: Theme.ColorTheme.bgMain
                            border.color: Theme.ColorTheme.border
                            border.width: 1
                            radius: 3
                        }
                    }

                    ScrollView {
                        id: cmdScrollView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                        ColumnLayout {
                            width: cmdScrollView.width
                            spacing: 4

                            Repeater {
                                model: viewModel ? viewModel.commandGroups : []

                                delegate: ColumnLayout {
                                    width: cmdScrollView.width
                                    spacing: 2

                                    Text {
                                        text: modelData.name + " (0x" + modelData.cmd.toString(16).toUpperCase() + ")"
                                        color: Theme.ColorTheme.textMain
                                        font.bold: true
                                        font.pixelSize: 11
                                        Layout.fillWidth: true
                                    }

                                    ColumnLayout {
                                        width: cmdScrollView.width
                                        spacing: 2

                                        Repeater {
                                            model: modelData.commands || []

                                            delegate: Rectangle {
                                                property var itemData: modelData
                                                Layout.fillWidth: true
                                                height: 44
                                                color: viewModel && viewModel.selectedCommand["id"] === itemData.id && viewModel.selectedCommand["cmd"] === itemData.cmd ? Theme.ColorTheme.btnNormal : "transparent"
                                                radius: 3
                                                border.color: viewModel && viewModel.selectedCommand["id"] === itemData.id && viewModel.selectedCommand["cmd"] === itemData.cmd ? Theme.ColorTheme.border : "transparent"
                                                border.width: viewModel && viewModel.selectedCommand["id"] === itemData.id && viewModel.selectedCommand["cmd"] === itemData.cmd ? 1 : 0

                                                ColumnLayout {
                                                    anchors.fill: parent
                                                    anchors.margins: 0
                                                    spacing: 2

                                                    RowLayout {
                                                        Layout.fillWidth: true
                                                        Layout.leftMargin: 6
                                                        Layout.rightMargin: 6
                                                        Layout.topMargin: 4

                                                        Text {
                                                            text: "0x" + (itemData.cmd !== undefined ? itemData.cmd.toString(16).toUpperCase() : "--") + "." + (itemData.subId !== undefined ? itemData.subId.toString(16).toUpperCase() : "--")
                                                            font.family: "Consolas"
                                                            font.bold: true
                                                            font.pixelSize: 13
                                                            color: Theme.ColorTheme.btnPrimary
                                                        }

                                                        Text {
                                                            text: itemData.label || ""
                                                            font.pixelSize: 13
                                                            font.bold: true
                                                            color: Theme.ColorTheme.textMain
                                                            Layout.fillWidth: true
                                                        }
                                                    }

                                                    Text {
                                                        text: itemData.description || ""
                                                        font.pixelSize: 11
                                                        color: Theme.ColorTheme.textSub
                                                        Layout.fillWidth: true
                                                        Layout.leftMargin: 6
                                                        Layout.rightMargin: 6
                                                        Layout.bottomMargin: 4
                                                        elide: Text.ElideRight
                                                    }
                                                }

                                                MouseArea {
                                                    anchors.fill: parent
                                                    onClicked: {
                                                        if (viewModel) viewModel.setSelectedCommand(itemData)
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: 220
                Layout.fillHeight: true
                color: Theme.ColorTheme.bgInner
                radius: 4
                border.color: Theme.ColorTheme.border
                border.width: 1

                Column {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 0

                    Text {
                        text: "ID数据标识"
                        color: Theme.ColorTheme.textMain
                        font.bold: true
                        font.pixelSize: 12
                        width: parent.width
                    }

                    Item { height: 8; width: parent.width }

                    Text {
                        text: viewModel && viewModel.selectedCommand && viewModel.selectedCommand["label"] !== undefined ? viewModel.selectedCommand["label"] : "请选择命令"
                        color: Theme.ColorTheme.textMain
                        font.bold: true
                        font.pixelSize: 13
                        width: parent.width
                    }

                    Item { height: 6; width: parent.width }

                    Rectangle {
                        width: parent.width
                        color: Theme.ColorTheme.bgMain
                        radius: 3
                        height: 52
                        border.color: Theme.ColorTheme.border
                        border.width: 1

                        Column {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 4

                            Row {
                                Text {
                                    text: "CMD"
                                    color: Theme.ColorTheme.textSub
                                    font.pixelSize: 11
                                    width: 40
                                }
                                Text {
                                    text: "0x" + (viewModel && viewModel.selectedCommand && viewModel.selectedCommand["cmd"] !== undefined ? viewModel.selectedCommand["cmd"].toString(16).toUpperCase().padStart(2, "0") : "--")
                                    font.family: "Consolas"
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: Theme.ColorTheme.btnPrimary
                                }
                            }

                            Row {
                                Text {
                                    text: "ID"
                                    color: Theme.ColorTheme.textSub
                                    font.pixelSize: 11
                                    width: 40
                                }
                                Text {
                                    text: "0x" + (viewModel && viewModel.selectedCommand && viewModel.selectedCommand["subId"] !== undefined ? viewModel.selectedCommand["subId"].toString(16).toUpperCase().padStart(2, "0") : "--")
                                    font.family: "Consolas"
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: Theme.ColorTheme.btnPrimary
                                }
                            }
                        }
                    }

                    Item { height: 8; width: parent.width }

                    Text {
                        text: viewModel && viewModel.selectedCommand && viewModel.selectedCommand["description"] !== undefined ? viewModel.selectedCommand["description"] : "暂无描述"
                        font.pixelSize: 11
                        color: Theme.ColorTheme.textSub
                        width: parent.width
                        wrapMode: Text.Wrap
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true

                color: Theme.ColorTheme.bgInner
                radius: 4

                border.color: Theme.ColorTheme.border
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    readonly property int labelWidth: 80

                    Text {
                        text: "协议指令详情"
                        color: Theme.ColorTheme.textMain
                        font.bold: true
                        font.pixelSize: 14
                    }

                    // ===========================
                    // 参数编辑
                    // ===========================
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 190
                        Layout.minimumHeight: 190

                        color: Theme.ColorTheme.bgMain
                        radius: 4

                        border.color: Theme.ColorTheme.border
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 8

                            Text {
                                text: "参数编辑"
                                font.bold: true
                                font.pixelSize: 12
                                color: Theme.ColorTheme.textMain
                            }

                            GridLayout {
                                Layout.fillWidth: true

                                columns: 2
                                rowSpacing: 8
                                columnSpacing: 12

                                Text {
                                    text: "指令名称:"
                                    color: Theme.ColorTheme.textSub
                                    Layout.preferredWidth: 80
                                }

                                Text {
                                    Layout.fillWidth: true

                                    text: viewModel
                                          && viewModel.selectedCommand
                                          && viewModel.selectedCommand["label"] !== undefined
                                          ? viewModel.selectedCommand["label"]
                                          : "请选择命令"

                                    color: Theme.ColorTheme.textMain
                                    font.bold: true

                                    elide: Text.ElideRight
                                }

                                Text {
                                    text: "操作符号:"
                                    color: Theme.ColorTheme.textSub
                                    Layout.preferredWidth: 80
                                }

                    TextField {
                        id: opInput

                        Layout.preferredWidth: 120

                        text: viewModel
                              ? viewModel.selectedOp.toString()
                              : "0"

                        font.family: "Consolas"

                        background: Rectangle {
                            radius: 3
                            color: Theme.ColorTheme.bgInner

                            border.color: Theme.ColorTheme.border
                            border.width: 1
                        }

                        onEditingFinished: {
                            if (viewModel)
                                viewModel.setSelectedOp(parseInt(text) || 0)
                        }
                    }

                    Text {
                        text: "数据1:"
                        color: Theme.ColorTheme.textSub
                    }

                    TextField {
                        id: data1Input

                        Layout.preferredWidth: 120

                        text: viewModel
                              ? viewModel.selectedData1.toString()
                              : "0"

                        font.family: "Consolas"

                        background: Rectangle {
                            radius: 3
                            color: Theme.ColorTheme.bgInner

                            border.color: Theme.ColorTheme.border
                            border.width: 1
                        }

                        onEditingFinished: {
                            if (viewModel)
                                viewModel.setSelectedData1(parseInt(text) || 0)
                        }
                    }

                    Text {
                        text: "HEX格式:"
                        color: Theme.ColorTheme.textSub
                    }

                    Text {
                        Layout.fillWidth: true

                        text: viewModel
                              ? viewModel.generatedHex
                              : ""

                        color: Theme.ColorTheme.btnPrimary

                        font.family: "Consolas"
                        font.pixelSize: 12

                        wrapMode: Text.WrapAnywhere
                    }
                }
            }
        }

        // ===========================
        // 参数说明
        // ===========================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            Layout.minimumHeight: 120

            color: Theme.ColorTheme.bgMain
            radius: 4

            border.color: Theme.ColorTheme.border
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8

                Text {
                    text: "参数说明"
                    font.bold: true
                    font.pixelSize: 12
                    color: Theme.ColorTheme.textMain
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    color: Theme.ColorTheme.bgInner
                    radius: 4

                    border.color: Theme.ColorTheme.border
                    border.width: 1

                    Text {
                        anchors.fill: parent
                        anchors.margins: 10

                        text: viewModel
                              && viewModel.selectedCommand
                              && viewModel.selectedCommand["description"] !== undefined
                              ? viewModel.selectedCommand["description"]
                              : "请选择命令"

                        color: Theme.ColorTheme.textSub

                        wrapMode: Text.WordWrap
                        verticalAlignment: Text.AlignTop
                    }
                }
            }
        }

        // ===========================
        // 操作区
        // ===========================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            Layout.minimumHeight: 80

            color: Theme.ColorTheme.bgMain
            radius: 4

            border.color: Theme.ColorTheme.border
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8

                Text {
                    text: "操作区"
                    font.bold: true
                    color: Theme.ColorTheme.textMain
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: "生成HEX"
                    Layout.preferredWidth: 100

                    onClicked: {
                        if (viewModel)
                            viewModel.generateHex()
                    }
                }

                Button {
                    text: "发送指令"
                    Layout.preferredWidth: 100

                    onClicked: {
                        if (viewModel
                                && viewModel.selectedCommand
                                && viewModel.selectedCommand["cmd"] !== undefined) {

                            viewModel.sendCommand(
                                        viewModel.selectedCommand["cmd"],
                                        viewModel.selectedCommand["subId"])
                        }
                    }
                }

                Button {
                    text: "一键复制"
                    Layout.preferredWidth: 100

                    onClicked: {
                        if (viewModel)
                            viewModel.copyHex()
                    }
                }

                Text {
                    id: statusText

                    text: ""

                    font.bold: true
                    color: Theme.ColorTheme.textSuccess

                    visible: text.length > 0
                }
            }
        }

        // ===========================
        // HEX结果
        // ===========================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 180

            color: Theme.ColorTheme.bgMain
            radius: 4

            border.color: Theme.ColorTheme.border
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8

                Text {
                    text: "HEX结果"
                    font.bold: true
                    color: Theme.ColorTheme.textMain
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    color: "#10151F"
                    radius: 4

                    border.color: "#2A3240"
                    border.width: 1

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 8

                        TextArea {
                            text: viewModel
                                  ? viewModel.generatedHex
                                  : ""

                            readOnly: true

                            wrapMode: TextArea.WrapAnywhere

                            font.family: "Consolas"
                            font.pixelSize: 14

                            color: "#6CFF6C"

                            selectByMouse: true

                            background: null
                        }
                    }
                }
            }
        }
    }
}
        }
    }

    Connections {
        target: viewModel
        function onCommandSent(cmd, id) {
            statusText.text = "已发送!"
            setTimeout(function() { statusText.text = "" }, 2000)
        }
        function onHexCopied() {
            statusText.text = "已复制!"
            setTimeout(function() { statusText.text = "" }, 2000)
        }
    }
}