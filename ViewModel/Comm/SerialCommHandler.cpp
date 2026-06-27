#include "SerialCommHandler.h"

SerialCommHandler::SerialCommHandler(QObject *parent)
    : CommHandler(parent)
    , m_baudRate(9600)
    , m_dataBits(8)
    , m_parity(0)
    , m_stopBits(0)
    , m_flowControl(0)
{
    qDebug() << "[SerialCommHandler] constructor, connecting signals";
    bool conn1 = connect(&m_serial, &SerialService::sigConnectStateChanged,
        this, &SerialCommHandler::onStateChanged);
    bool conn2 = connect(&m_serial, &SerialService::sigRecvData,
        this, &SerialCommHandler::onRecvData);
    bool conn3 = connect(&m_serial, &SerialService::sigErrorOccurred,
        this, &SerialCommHandler::onErrorOccurred);
    qDebug() << "[SerialCommHandler] signal connections - state:" << conn1 
             << ", data:" << conn2 << ", error:" << conn3;
}

SerialCommHandler::~SerialCommHandler()
{
    m_serial.closePort();
}

QString SerialCommHandler::type() const
{
    return "serial";
}

CommConnectState SerialCommHandler::getConnectState() const
{
    return m_serial.getConnectState();
}

bool SerialCommHandler::sendData(const QByteArray &data)
{
    return m_serial.sendData(data);
}

void SerialCommHandler::closePort()
{
    m_serial.closePort();
}

bool SerialCommHandler::openPort(const QVariantMap &params)
{
    m_portName = params.value("portName").toString();
    m_baudRate = params.value("baudRate").toInt();
    m_dataBits = params.value("dataBits").toInt();
    m_parity = params.value("parity").toInt();
    m_stopBits = params.value("stopBits").toInt();
    m_flowControl = params.value("flowControl").toInt();

    m_serial.setPortName(m_portName);
    m_serial.setBaudRate(baudRateFromInt(m_baudRate));
    m_serial.setSerialParams(dataBitsFromInt(m_dataBits),
                             parityFromInt(m_parity),
                             stopBitsFromInt(m_stopBits),
                             flowControlFromInt(m_flowControl));

    return m_serial.openPort();
}

QVariantMap SerialCommHandler::getCurrentParams() const
{
    QVariantMap params;
    params["portName"] = m_portName;
    params["baudRate"] = m_baudRate;
    params["dataBits"] = m_dataBits;
    params["parity"] = m_parity;
    params["stopBits"] = m_stopBits;
    params["flowControl"] = m_flowControl;
    return params;
}

QStringList SerialCommHandler::getAvailableDevices() const
{
    QStringList ports = m_serial.getAvailablePortList();
    qDebug() << "[SerialCommHandler] getAvailableDevices returned:" << ports;
    return ports;
}

void SerialCommHandler::onStateChanged(CommConnectState state)
{
    emit connectStateChanged(state);
}

void SerialCommHandler::onRecvData(const QByteArray &data)
{
    qDebug() << "[SerialCommHandler] onRecvData, size:" << data.size() 
             << ", hex:" << data.toHex().left(32);
    emit dataReceived(data);
}

void SerialCommHandler::onErrorOccurred(const QString &message)
{
    emit errorOccurred(message);
}

SerialBaudRate SerialCommHandler::baudRateFromInt(int baud) const
{
    switch (baud) {
    case 1200: return SerialBaudRate::BR_1200;
    case 2400: return SerialBaudRate::BR_2400;
    case 4800: return SerialBaudRate::BR_4800;
    case 9600: return SerialBaudRate::BR_9600;
    case 19200: return SerialBaudRate::BR_19200;
    case 38400: return SerialBaudRate::BR_38400;
    case 57600: return SerialBaudRate::BR_57600;
    case 115200: return SerialBaudRate::BR_115200;
    case 230400: return SerialBaudRate::BR_230400;
    case 460800: return SerialBaudRate::BR_460800;
    case 921600: return SerialBaudRate::BR_921600;
    default: return SerialBaudRate::BR_9600;
    }
}

SerialDataBits SerialCommHandler::dataBitsFromInt(int bits) const
{
    switch (bits) {
    case 5: return SerialDataBits::D5;
    case 6: return SerialDataBits::D6;
    case 7: return SerialDataBits::D7;
    case 8: return SerialDataBits::D8;
    default: return SerialDataBits::D8;
    }
}

SerialParity SerialCommHandler::parityFromInt(int index) const
{
    switch (index) {
    case 0: return SerialParity::None;
    case 1: return SerialParity::Odd;
    case 2: return SerialParity::Even;
    case 3: return SerialParity::Mark;
    case 4: return SerialParity::Space;
    default: return SerialParity::None;
    }
}

SerialStopBits SerialCommHandler::stopBitsFromInt(int index) const
{
    switch (index) {
    case 0: return SerialStopBits::S1;
    case 1: return SerialStopBits::S1_5;
    case 2: return SerialStopBits::S2;
    default: return SerialStopBits::S1;
    }
}

SerialFlowControl SerialCommHandler::flowControlFromInt(int index) const
{
    switch (index) {
    case 0: return SerialFlowControl::None;
    case 1: return SerialFlowControl::RTS_CTS;
    case 2: return SerialFlowControl::XON_XOFF;
    default: return SerialFlowControl::None;
    }
}
