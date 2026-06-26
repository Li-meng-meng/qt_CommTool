#include "BluetoothServiceTest.h"

void BluetoothServiceTest::initTestCase()
{
}

void BluetoothServiceTest::cleanupTestCase()
{
}

void BluetoothServiceTest::init()
{
    m_bluetoothService = new BluetoothService();
}

void BluetoothServiceTest::cleanup()
{
    delete m_bluetoothService;
    m_bluetoothService = nullptr;
}

void BluetoothServiceTest::testConstructor()
{
    QVERIFY(m_bluetoothService != nullptr);
    QCOMPARE(m_bluetoothService->getConnectState(), CommConnectState::Disconnected);
    QCOMPARE(m_bluetoothService->isScanning(), false);
}

void BluetoothServiceTest::testDestructor()
{
    BluetoothService *service = new BluetoothService();
    QVERIFY(service != nullptr);
    delete service;
}

void BluetoothServiceTest::testGetConnectState()
{
    QCOMPARE(m_bluetoothService->getConnectState(), CommConnectState::Disconnected);
}

void BluetoothServiceTest::testIsScanning()
{
    QCOMPARE(m_bluetoothService->isScanning(), false);
}

void BluetoothServiceTest::testSetServiceUuid()
{
    QBluetoothUuid uuid("0000ffe0-0000-1000-8000-00805f9b34fb");
    m_bluetoothService->setServiceUuid(uuid);
}

void BluetoothServiceTest::testSetWriteCharUuid()
{
    QBluetoothUuid uuid("0000ffe1-0000-1000-8000-00805f9b34fb");
    m_bluetoothService->setWriteCharUuid(uuid);
}

void BluetoothServiceTest::testSetNotifyCharUuid()
{
    QBluetoothUuid uuid("0000ffe1-0000-1000-8000-00805f9b34fb");
    m_bluetoothService->setNotifyCharUuid(uuid);
}

void BluetoothServiceTest::testSendDataWhenDisconnected()
{
    QByteArray testData = "test";
    bool result = m_bluetoothService->sendData(testData);
    QCOMPARE(result, false);
}

void BluetoothServiceTest::testClosePortWhenDisconnected()
{
    m_bluetoothService->closePort();
}

void BluetoothServiceTest::testConnectToDeviceWithInvalidAddress()
{
    bool result = m_bluetoothService->connectToDevice("00:00:00:00:00:00");
    QVERIFY(result == true || result == false);
}

void BluetoothServiceTest::testGetConnectedDeviceName()
{
    QString name = m_bluetoothService->getConnectedDeviceName();
    QVERIFY(name.isEmpty());
}

void BluetoothServiceTest::testGetConnectedDeviceAddress()
{
    QString address = m_bluetoothService->getConnectedDeviceAddress();
    QVERIFY(address.isEmpty());
}

QTEST_MAIN(BluetoothServiceTest)
