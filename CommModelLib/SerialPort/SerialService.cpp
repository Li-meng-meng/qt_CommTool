#include "SerialService.h"

SerialService::SerialService(QObject *parent)
    : BaseCommService(parent)
    , m_baudRate(SerialBaudRate::BR_9600)
    , m_dataBits(SerialDataBits::D8)
    , m_parity(SerialParity::None)
    , m_stopBits(SerialStopBits::S1)
    , m_flowControl(SerialFlowControl::None)
    , m_connectState(CommConnectState::Disconnected)
{
    connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialService::onReadyRead);
    connect(&m_serialPort, &QSerialPort::errorOccurred, this, &SerialService::onErrorOccurred);
}

SerialService::~SerialService()
{
    closePort();
}

CommConnectState SerialService::getConnectState() const
{
    return m_connectState;
}

void SerialService::closePort()
{
    if (m_serialPort.isOpen()) {
        m_serialPort.close();
    }
    m_connectState = CommConnectState::Disconnected;
    emit sigConnectStateChanged(m_connectState);
}

bool SerialService::sendData(const QByteArray &data)
{
    if (!m_serialPort.isOpen()) {
        emit sigErrorOccurred(tr("Port is not open"));
        return false;
    }

    qint64 bytesWritten = m_serialPort.write(data);
    return bytesWritten == data.size();
}

QStringList SerialService::getAvailablePortList() const
{
    QStringList ports;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        ports.append(info.portName());
    }
    return ports;
}

void SerialService::setPortName(const QString &port)
{
    m_portName = port;
}

void SerialService::setBaudRate(SerialBaudRate baud)
{
    m_baudRate = baud;
}

void SerialService::setSerialParams(SerialDataBits db, SerialParity pr, SerialStopBits sb, SerialFlowControl fc)
{
    m_dataBits = db;
    m_parity = pr;
    m_stopBits = sb;
    m_flowControl = fc;
}

QVariantMap SerialService::getSerialParams() const
{
    QVariantMap params;
    params["portName"] = m_portName;
    params["baudRate"] = static_cast<int>(m_baudRate);
    params["dataBits"] = static_cast<int>(m_dataBits);
    params["parity"] = static_cast<int>(m_parity);
    params["stopBits"] = static_cast<int>(m_stopBits);
    params["flowControl"] = static_cast<int>(m_flowControl);
    return params;
}

bool SerialService::openPort()
{
    qDebug() << "=== SerialService::openPort START ===";
    qDebug() << "  m_portName:" << m_portName;

    if (m_portName.isEmpty()) {
        qDebug() << "  ERROR: portName is empty";
        emit sigErrorOccurred(tr("Port name is empty"));
        qDebug() << "=== SerialService::openPort END (empty port) ===";
        return false;
    }

    qDebug() << "  Setting state to Connecting...";
    m_connectState = CommConnectState::Connecting;
    emit sigConnectStateChanged(m_connectState);

    qDebug() << "  Setting port name on QSerialPort...";
    m_serialPort.setPortName(m_portName);

    qDebug() << "  Setting baudRate:" << static_cast<int>(m_baudRate);
    switch (m_baudRate) {
    case SerialBaudRate::BR_1200:
        m_serialPort.setBaudRate(QSerialPort::Baud1200);
        break;
    case SerialBaudRate::BR_2400:
        m_serialPort.setBaudRate(QSerialPort::Baud2400);
        break;
    case SerialBaudRate::BR_4800:
        m_serialPort.setBaudRate(QSerialPort::Baud4800);
        break;
    case SerialBaudRate::BR_9600:
        m_serialPort.setBaudRate(QSerialPort::Baud9600);
        break;
    case SerialBaudRate::BR_19200:
        m_serialPort.setBaudRate(QSerialPort::Baud19200);
        break;
    case SerialBaudRate::BR_38400:
        m_serialPort.setBaudRate(QSerialPort::Baud38400);
        break;
    case SerialBaudRate::BR_57600:
        m_serialPort.setBaudRate(QSerialPort::Baud57600);
        break;
    case SerialBaudRate::BR_115200:
        m_serialPort.setBaudRate(QSerialPort::Baud115200);
        break;
    case SerialBaudRate::BR_230400:
        m_serialPort.setBaudRate(230400);
        break;
    case SerialBaudRate::BR_460800:
        m_serialPort.setBaudRate(460800);
        break;
    case SerialBaudRate::BR_921600:
        m_serialPort.setBaudRate(921600);
        break;
    }

    qDebug() << "  Setting dataBits:" << static_cast<int>(m_dataBits);
    switch (m_dataBits) {
    case SerialDataBits::D5:
        m_serialPort.setDataBits(QSerialPort::Data5);
        break;
    case SerialDataBits::D6:
        m_serialPort.setDataBits(QSerialPort::Data6);
        break;
    case SerialDataBits::D7:
        m_serialPort.setDataBits(QSerialPort::Data7);
        break;
    case SerialDataBits::D8:
        m_serialPort.setDataBits(QSerialPort::Data8);
        break;
    }

    qDebug() << "  Setting parity:" << static_cast<int>(m_parity);
    switch (m_parity) {
    case SerialParity::None:
        m_serialPort.setParity(QSerialPort::NoParity);
        break;
    case SerialParity::Odd:
        m_serialPort.setParity(QSerialPort::OddParity);
        break;
    case SerialParity::Even:
        m_serialPort.setParity(QSerialPort::EvenParity);
        break;
    case SerialParity::Mark:
        m_serialPort.setParity(QSerialPort::MarkParity);
        break;
    case SerialParity::Space:
        m_serialPort.setParity(QSerialPort::SpaceParity);
        break;
    }

    qDebug() << "  Setting stopBits:" << static_cast<int>(m_stopBits);
    switch (m_stopBits) {
    case SerialStopBits::S1:
        m_serialPort.setStopBits(QSerialPort::OneStop);
        break;
    case SerialStopBits::S1_5:
        m_serialPort.setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case SerialStopBits::S2:
        m_serialPort.setStopBits(QSerialPort::TwoStop);
        break;
    }

    qDebug() << "  Setting flowControl:" << static_cast<int>(m_flowControl);
    switch (m_flowControl) {
    case SerialFlowControl::None:
        m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
        break;
    case SerialFlowControl::RTS_CTS:
        m_serialPort.setFlowControl(QSerialPort::HardwareControl);
        break;
    case SerialFlowControl::XON_XOFF:
        m_serialPort.setFlowControl(QSerialPort::SoftwareControl);
        break;
    }

    qDebug() << "  Calling QSerialPort::open()...";
    if (!m_serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "  ERROR: QSerialPort::open() failed";
        qDebug() << "  Error string:" << m_serialPort.errorString();
        qDebug() << "  Error code:" << m_serialPort.error();

        m_connectState = CommConnectState::Error;
        emit sigConnectStateChanged(m_connectState);
        emit sigErrorOccurred(m_serialPort.errorString());
        qDebug() << "=== SerialService::openPort END (open failed) ===";
        return false;
    }

    qDebug() << "  QSerialPort::open() successful";
    m_connectState = CommConnectState::Connected;
    emit sigConnectStateChanged(m_connectState);
    qDebug() << "=== SerialService::openPort END (success) ===";
    return true;
}

void SerialService::onReadyRead()
{
    QByteArray data = m_serialPort.readAll();
    if (!data.isEmpty()) {
        emit sigRecvData(data);
    }
}

void SerialService::onErrorOccurred(QSerialPort::SerialPortError error)
{
    qDebug() << "=== SerialService::onErrorOccurred START ===";
    qDebug() << "  Error code:" << error;
    qDebug() << "  Error string:" << m_serialPort.errorString();

    if (error == QSerialPort::NoError) {
        qDebug() << "  No error, ignoring";
        qDebug() << "=== SerialService::onErrorOccurred END (no error) ===";
        return;
    }

    if (error == QSerialPort::ResourceError ||
        error == QSerialPort::PermissionError ||
        error == QSerialPort::OpenError ||
        error == QSerialPort::DeviceNotFoundError) {
        qDebug() << "  Serious error detected, closing port...";
        if (m_serialPort.isOpen()) {
            m_serialPort.close();
            qDebug() << "  Port closed";
        }
        m_connectState = CommConnectState::Disconnected;
        emit sigConnectStateChanged(m_connectState);
        emit sigErrorOccurred(m_serialPort.errorString());
        qDebug() << "  State reset to Disconnected";
    } else {
        qDebug() << "  Non-critical error, setting state to Error";
        m_connectState = CommConnectState::Error;
        emit sigConnectStateChanged(m_connectState);
        emit sigErrorOccurred(m_serialPort.errorString());
    }

    qDebug() << "=== SerialService::onErrorOccurred END ===";
}