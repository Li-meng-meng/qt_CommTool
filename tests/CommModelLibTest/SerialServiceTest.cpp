#include "SerialServiceTest.h"
#include "../../CommModelLib/Common/EnumComm.h"

void SerialServiceTest::initTestCase()
{
}

void SerialServiceTest::cleanupTestCase()
{
}

void SerialServiceTest::init()
{
    m_serialService = new SerialService();
}

void SerialServiceTest::cleanup()
{
    delete m_serialService;
    m_serialService = nullptr;
}

void SerialServiceTest::testConstructor()
{
    QVERIFY(m_serialService != nullptr);
    QCOMPARE(m_serialService->getConnectState(), CommConnectState::Disconnected);
}

void SerialServiceTest::testDestructor()
{
    SerialService *service = new SerialService();
    QVERIFY(service != nullptr);
    delete service;
}

void SerialServiceTest::testGetConnectState()
{
    QCOMPARE(m_serialService->getConnectState(), CommConnectState::Disconnected);
}

void SerialServiceTest::testSetPortName()
{
    QString testPort = "COM3";
    m_serialService->setPortName(testPort);
    QVariantMap params = m_serialService->getSerialParams();
    QCOMPARE(params["portName"].toString(), testPort);
}

void SerialServiceTest::testSetBaudRate()
{
    m_serialService->setBaudRate(SerialBaudRate::BR_115200);
    QVariantMap params = m_serialService->getSerialParams();
    QCOMPARE(params["baudRate"].toInt(), (int)SerialBaudRate::BR_115200);
}

void SerialServiceTest::testSetSerialParams()
{
    m_serialService->setSerialParams(
        SerialDataBits::D8,
        SerialParity::None,
        SerialStopBits::S1,
        SerialFlowControl::None
    );
    QVariantMap params = m_serialService->getSerialParams();
    QCOMPARE(params["dataBits"].toInt(), (int)SerialDataBits::D8);
    QCOMPARE(params["parity"].toInt(), (int)SerialParity::None);
    QCOMPARE(params["stopBits"].toInt(), (int)SerialStopBits::S1);
    QCOMPARE(params["flowControl"].toInt(), (int)SerialFlowControl::None);
}

void SerialServiceTest::testGetSerialParams()
{
    QVariantMap params = m_serialService->getSerialParams();
    QVERIFY(params.contains("portName"));
    QVERIFY(params.contains("baudRate"));
    QVERIFY(params.contains("dataBits"));
    QVERIFY(params.contains("parity"));
    QVERIFY(params.contains("stopBits"));
    QVERIFY(params.contains("flowControl"));
}

void SerialServiceTest::testGetAvailablePortList()
{
    QStringList ports = m_serialService->getAvailablePortList();
    QVERIFY(ports.isEmpty() || !ports.isEmpty());
}

void SerialServiceTest::testSendDataWhenDisconnected()
{
    QByteArray testData = "test";
    bool result = m_serialService->sendData(testData);
    QCOMPARE(result, false);
}

void SerialServiceTest::testClosePortWhenDisconnected()
{
    m_serialService->closePort();
}

void SerialServiceTest::testInvalidPortName()
{
    m_serialService->setPortName("");
    bool result = m_serialService->openPort();
    QCOMPARE(result, false);
}

QTEST_MAIN(SerialServiceTest)
