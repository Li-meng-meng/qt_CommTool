import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./Theme" as Theme

Window {
    id: root
    title: qsTr("应用设置")
    visible: false
    width: 500
    height: 300
    flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint
    color: Theme.ColorTheme.bgMain

    property bool isDarkMode: false

    ColumnLayout {
        anchors.fill: parent

        // 标题栏
        Rectangle {
            id: titleBar
            height: 36
            Layout.fillWidth: true
            color: Theme.ColorTheme.bgInner
            border.width: 0

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: Theme.ColorTheme.border
            }

            // 拖动区域
            MouseArea {
                anchors.fill: parent
                preventStealing: true
                property point clickPos: "0,0"

                onPressed: {
                    clickPos = Qt.point(mouse.x, mouse.y)
                }

                onPositionChanged: {
                    if (mouse.buttons & Qt.LeftButton) {
                        root.x += mouse.x - clickPos.x
                        root.y += mouse.y - clickPos.y
                    }
                }
            }

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                Label {
                    text: qsTr("应用设置")
                    color: Theme.ColorTheme.textMain
                    font.bold: true
                    font.pointSize: 13
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        // 内容区域
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20
            anchors.margins: 20

            GroupBox {
                title: qsTr("主题设置")
                Layout.fillWidth: true

                background: Rectangle {
                    color: Theme.ColorTheme.bgInner
                    border.width: 1
                    border.color: Theme.ColorTheme.border
                    radius: 4
                }

                label: Label {
                    color: Theme.ColorTheme.textMain
                    font.bold: true
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        Label {
                            text: qsTr("暗黑模式")
                            color: Theme.ColorTheme.textMain
                            Layout.preferredWidth: 100
                        }

                        Switch {
                            id: darkModeSwitch
                            checked: root.isDarkMode
                            onCheckedChanged: {
                                root.isDarkMode = checked
                                root.isDarkModeChanged(checked)
                            }
                        }

                        Label {
                            text: root.isDarkMode ? qsTr("开启") : qsTr("关闭")
                            color: Theme.ColorTheme.textSub
                        }
                    }

                    Label {
                        text: qsTr("切换暗黑模式将改变应用的整体外观")
                        color: Theme.ColorTheme.textSub
                        font.pointSize: 10
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }

            // 底部按钮
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("关闭")
                    onClicked: root.visible = false

                    background: Rectangle {
                        color: parent.pressed ? Theme.ColorTheme.btnNormalPressed :
                               parent.hovered ? Theme.ColorTheme.btnNormalHover :
                               Theme.ColorTheme.btnNormal
                        border.width: 1
                        border.color: Theme.ColorTheme.btnNormalBorder
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: Theme.ColorTheme.btnNormalText
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}