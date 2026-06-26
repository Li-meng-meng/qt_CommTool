#ifndef SERIALSERVICETEST_H
#define SERIALSERVICETEST_H

#include <QtTest/QTest>
#include "../../CommModelLib/SerialPort/SerialService.h"

class SerialServiceTest : public QObject
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
    void testSetPortName();
    void testSetBaudRate();
    void testSetSerialParams();
    void testGetSerialParams();
    void testGetAvailablePortList();
    void testSendDataWhenDisconnected();
    void testClosePortWhenDisconnected();
    void testInvalidPortName();

private:
    SerialService *m_serialService;
};

#endif // SERIALSERVICETEST_H
