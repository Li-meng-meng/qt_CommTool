import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CommTool.ViewModel 1.0
import "./Theme" as Theme

Window {
    id: root
    width: 1024
    height: 768
    visible: true
    title: qsTr("CommTool")
    color: Theme.ColorTheme.bgMain

    // Current page state: 0 = Serial, 1 = DataPlot
    property int currentPage: 0

    CommViewModel {
        id: commViewModel
        onConnectStateChanged: {
            if (state === 2) {
                toast.show("串口已打开", "success")
            } else if (state === 0) {
                toast.show("串口已关闭", "info")
            }
        }
        onErrorOccurred: {
            toast.show(message, "error")
        }
    }

    AppSettingsViewModel {
        id: appSettingsViewModel
    }

    Theme.Toast {
        id: toast
        z: 100

        Component.onCompleted: {
            toast.show("CommTool启动成功", "success")
        }
    }

    Rectangle {
        anchors.fill: parent

        // Sidebar
        SideBarView {
            id: sideBar
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 200
            onOpenSettings: settingsDialog.visible = true
            onSwitchToDataPlot: {
                root.currentPage = 1
                commViewModel.isPlotViewActive = true
                commViewModel.dataPlotViewModel.isViewActive = true
                commViewModel.isShowViewActive = false
            }
            onSwitchToCommand: {
                root.currentPage = 2
                commViewModel.isPlotViewActive = false
                commViewModel.dataPlotViewModel.isViewActive = false
                commViewModel.isShowViewActive = false
            }
            onSwitchToSerial: {
                root.currentPage = 0
                settingView.currentCommType = "serial"
                commViewModel.commManager.setCurrentType("serial")
                commViewModel.isPlotViewActive = false
                commViewModel.dataPlotViewModel.isViewActive = false
                commViewModel.isShowViewActive = true
            }
            onSwitchToBluetooth: {
                root.currentPage = 0
                settingView.currentCommType = "bluetooth"
                commViewModel.commManager.setCurrentType("bluetooth")
                commViewModel.isPlotViewActive = false
                commViewModel.dataPlotViewModel.isViewActive = false
                commViewModel.isShowViewActive = true
            }
        }

        // Main Content Area
        Item {
            anchors.left: sideBar.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            // Serial Port Page
            Item {
                id: serialPage
                visible: root.currentPage === 0
                anchors.fill: parent

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    SettingView {
                        id: settingView
                        height: 300

                        portList: commViewModel.commManager.deviceList
                        bluetoothDeviceList: commViewModel.commManager.deviceList
                        isScanning: commViewModel.commManager.isScanning
                        connectState: commViewModel.connectState

                        onOpenPort: commViewModel.commManager.openPort({
                            "portName": portName,
                            "baudRate": baudRate,
                            "dataBits": dataBits,
                            "parity": parity,
                            "stopBits": stopBits,
                            "flowControl": flowControl
                        })
                        onOpenBluetoothPort: commViewModel.commManager.openPort({
                            "deviceAddress": deviceAddress,
                            "serviceUuid": serviceUuid,
                            "writeCharUuid": writeCharUuid,
                            "notifyCharUuid": notifyCharUuid
                        })
                        onClosePort: commViewModel.commManager.closePort()
                        onRefreshPorts: commViewModel.commManager.refreshDevices()
                        onStartScan: commViewModel.commManager.startScan()
                        onStopScan: commViewModel.commManager.stopScan()
                    }

                    DataShowView {
                        id: dataShowView
                        height: root.height - 300
                        recvData: commViewModel.receiveData
                        hexDisplay: commViewModel.hexDisplay
                        hexSend: commViewModel.hexSend

                        onSendData: function(data) {
                            commViewModel.sendData(data)
                        }
                        onClearReceive: commViewModel.clearReceiveData()

                        onHexDisplayChanged: commViewModel.hexDisplay = hexDisplay
                        onHexSendChanged: commViewModel.hexSend = hexSend
                    }
                }
            }

            // Data Plot Page
            Item {
                id: dataPlotPage
                visible: root.currentPage === 1
                anchors.fill: parent

                DataPlotView {
                    anchors.fill: parent
                    dataPlotViewModel: commViewModel.dataPlotViewModel
                    commandViewModel: commViewModel.commandViewModel
                    toast: toast
                }
            }

            // Command Page
            Item {
                id: commandPage
                visible: root.currentPage === 2
                anchors.fill: parent

                CommandMenuView {
                    anchors.fill: parent
                    viewModel: commViewModel.commandViewModel
                }
            }
        }
    }

    signal sendData(string data)

    // 设置对话框
    AppSettingsDialog {
        id: settingsDialog
        isDarkMode: appSettingsViewModel.isDarkMode
        onIsDarkModeChanged: {
            appSettingsViewModel.isDarkMode = settingsDialog.isDarkMode
            Theme.ColorTheme.isDark = settingsDialog.isDarkMode
        }
    }
}