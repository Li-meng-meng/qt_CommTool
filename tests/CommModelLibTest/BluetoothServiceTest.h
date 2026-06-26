#ifndef BLUETOOTHSERVICETEST_H
#define BLUETOOTHSERVICETEST_H

#include <QtTest/QTest>
#include "../../CommModelLib/Bluetooth/BluetoothService.h"

class BluetoothServiceTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructor();
    void testDestructor();
    void testGetConnectState();
    void testIsScanning();
    void testSetServiceUuid();
    void testSetWriteCharUuid();
    void testSetNotifyCharUuid();
    void testSendDataWhenDisconnected();
    void testClosePortWhenDisconnected();
    void testConnectToDeviceWithInvalidAddress();
    void testGetConnectedDeviceName();
    void testGetConnectedDeviceAddress();

private:
    BluetoothService *m_bluetoothService;
};

#endif // BLUETOOTHSERVICETEST_H
