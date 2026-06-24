import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: root
    title: qsTr("Help")
    width: 600
    height: 400
    modality: Qt.NonModal

    Text {
        text: qsTr("CommTool - Serial Port Debug Tool\n\nFeatures:\n- Serial port communication\n- Data send and receive\n- Port parameter configuration\n\nUsage:\n1. Select a serial port from the dropdown\n2. Configure baud rate and other parameters\n3. Click 'Open' to connect\n4. Enter data in the send field and click 'Send'\n5. Received data will appear in the receive area\n\nVersion: 1.0.0")
        anchors.fill: parent
        padding: 10
        font.pixelSize: 14
    }
}