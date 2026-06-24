#include "CommViewModel.h"
#include <QRegularExpression>
#include <QDateTime>
#include <QDebug>

CommViewModel::CommViewModel(QObject *parent)
    : QObject(parent)
    , m_baudRate(9600)
    , m_dataBits(8)
    , m_parity(0)
    , m_stopBits(0)
    , m_flowControl(0)
    , m_hexDisplay(true)
    , m_hexSend(true)
    , m_parseEnabled(true)
    , m_isPlotViewActive(false)
    , m_isShowViewActive(true)
    , m_dataParser(new DataParser(this))
    , m_dataPlotViewModel(new DataPlotViewModel(this))
{
    // 串口信号连接
    connect(&m_serial, &SerialService::sigConnectStateChanged,
        this, &CommViewModel::onStateChanged);
    connect(&m_serial, &SerialService::sigRecvData,
        this, &CommViewModel::onRecvData);
    connect(&m_serial, &SerialService::sigErrorOccurred,
        this, &CommViewModel::onErrorOccurred);

    // DataParser 信号连接
    connect(m_dataParser, &DataParser::newAccelData,
        this, &CommViewModel::onNewAccelData);
    connect(m_dataParser, &DataParser::newGyroData,
        this, &CommViewModel::onNewGyroData);
    connect(m_dataParser, &DataParser::newSensorData,
        this, &CommViewModel::onNewSensorData);
    connect(m_dataParser, &DataParser::newSensorDataPacket,
        this, &CommViewModel::onNewSensorDataPacket);
    connect(m_dataParser, &DataParser::newAllData,
        this, &CommViewModel::onNewAllData);
    connect(m_dataParser, &DataParser::parseError,
        this, &CommViewModel::onParseError);

    refreshPortList();
}

CommViewModel::~CommViewModel()
{
    qDebug() << "=== CommViewModel destructor START ===";
    m_serial.closePort();
    qDebug() << "=== CommViewModel destructor END ===";
}

QString CommViewModel::getPortName() const
{
    return m_portName;
}

void CommViewModel::setPortName(const QString& name)
{
    if (m_portName != name) {
        m_portName = name;
        emit portNameChanged(m_portName);
    }
}

int CommViewModel::getBaudRate() const
{
    return m_baudRate;
}

void CommViewModel::setBaudRate(int baudRate)
{
    if (m_baudRate != baudRate) {
        m_baudRate = baudRate;
        emit baudRateChanged(m_baudRate);
    }
}

int CommViewModel::getDataBits() const
{
    return m_dataBits;
}

void CommViewModel::setDataBits(int dataBits)
{
    if (m_dataBits != dataBits) {
        m_dataBits = dataBits;
        emit dataBitsChanged(m_dataBits);
    }
}

int CommViewModel::getParity() const
{
    return m_parity;
}

void CommViewModel::setParity(int parity)
{
    if (m_parity != parity) {
        m_parity = parity;
        emit parityChanged(m_parity);
    }
}

int CommViewModel::getStopBits() const
{
    return m_stopBits;
}

void CommViewModel::setStopBits(int stopBits)
{
    if (m_stopBits != stopBits) {
        m_stopBits = stopBits;
        emit stopBitsChanged(m_stopBits);
    }
}

int CommViewModel::getFlowControl() const
{
    return m_flowControl;
}

void CommViewModel::setFlowControl(int flowControl)
{
    if (m_flowControl != flowControl) {
        m_flowControl = flowControl;
        emit flowControlChanged(m_flowControl);
    }
}

int CommViewModel::getConnectState() const
{
    return static_cast<int>(m_serial.getConnectState());
}

QString CommViewModel::getReceiveData() const
{
    return m_receiveData;
}

QString CommViewModel::getSendHistory() const
{
    return m_sendHistory;
}

QStringList CommViewModel::getPortList() const
{
    return m_portList;
}

bool CommViewModel::getHexDisplay() const
{
    return m_hexDisplay;
}

void CommViewModel::setHexDisplay(bool hex)
{
    if (m_hexDisplay != hex) {
        m_hexDisplay = hex;
        emit hexDisplayChanged(m_hexDisplay);
    }
}

bool CommViewModel::getHexSend() const
{
    return m_hexSend;
}

void CommViewModel::setHexSend(bool hex)
{
    if (m_hexSend != hex) {
        m_hexSend = hex;
        emit hexSendChanged(m_hexSend);
    }
}

bool CommViewModel::getParseEnabled() const
{
    return m_parseEnabled;
}

void CommViewModel::setParseEnabled(bool enabled)
{
    if (m_parseEnabled != enabled) {
        m_parseEnabled = enabled;
        emit parseEnabledChanged(m_parseEnabled);
    }
}

bool CommViewModel::getIsPlotViewActive() const
{
    return m_isPlotViewActive;
}

void CommViewModel::setIsPlotViewActive(bool active)
{
    if (m_isPlotViewActive != active) {
        m_isPlotViewActive = active;
        emit isPlotViewActiveChanged(m_isPlotViewActive);
    }
}

bool CommViewModel::getIsShowViewActive() const
{
    return m_isShowViewActive;
}

void CommViewModel::setIsShowViewActive(bool active)
{
    if (m_isShowViewActive != active) {
        m_isShowViewActive = active;
        emit isShowViewActiveChanged(m_isShowViewActive);
    }
}

DataPlotViewModel* CommViewModel::getDataPlotViewModel() const
{
    return m_dataPlotViewModel;
}

void CommViewModel::openSerialPort(const QString& portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl)
{
    qDebug() << "=== CommViewModel::openSerialPort START ===";
    qDebug() << "  Port:" << portName;
    qDebug() << "  BaudRate:" << baudRate;

    // 设置串口参数
    m_serial.setPortName(portName);
    m_serial.setBaudRate(baudRateFromInt(baudRate));
    m_serial.setSerialParams(dataBitsFromInt(dataBits),
                             parityFromInt(parity),
                             stopBitsFromInt(stopBits),
                             flowControlFromInt(flowControl));

    // 打开串口
    bool ret = m_serial.openPort();

    qDebug() << "  openPort returned:" << ret;
    qDebug() << "=== CommViewModel::openSerialPort END ===";
}

void CommViewModel::closeSerialPort()
{
    qDebug() << "=== CommViewModel::closeSerialPort START ===";
    m_serial.closePort();
    qDebug() << "=== CommViewModel::closeSerialPort END ===";
}

void CommViewModel::sendData(const QString& data)
{
    qDebug() << "=== CommViewModel::sendData START ===";
    qDebug() << "  Input data:" << data;
    qDebug() << "  hexSend:" << m_hexSend;

    QByteArray sendData;
    if (m_hexSend) {
        qDebug() << "  Converting from hex...";
        sendData = hexStringToByteArray(data);
    } else {
        qDebug() << "  Converting from UTF-8...";
        sendData = data.toUtf8();
    }

    qDebug() << "  Data to send size:" << sendData.size();
    qDebug() << "  Data to send (hex):" << byteArrayToHexString(sendData);

    qDebug() << "  Calling m_serial.sendData()...";
    bool ret = m_serial.sendData(sendData);
    qDebug() << "  m_serial.sendData() returned:" << ret;

    if (ret) {
        QString displayData = m_hexSend ? byteArrayToHexString(sendData) : data;
        m_sendHistory += displayData + "\n";
        emit sendHistoryChanged(m_sendHistory);
        qDebug() << "  Send successful, added to history";
    } else {
        qDebug() << "  Send failed";
        emit errorOccurred(tr("Failed to send data"));
    }

    qDebug() << "=== CommViewModel::sendData END ===";
}

void CommViewModel::refreshPortList()
{
    m_portList = m_serial.getAvailablePortList();
    emit portListChanged(m_portList);
}

void CommViewModel::clearReceiveData()
{
    m_receiveData.clear();
    emit receiveDataChanged(m_receiveData);
}

void CommViewModel::clearSendHistory()
{
    m_sendHistory.clear();
    emit sendHistoryChanged(m_sendHistory);
}

void CommViewModel::onStateChanged(CommConnectState state)
{
    qDebug() << "=== CommViewModel::onStateChanged START ===";
    qDebug() << "  New state:" << static_cast<int>(state);
    qDebug() << "  Old state:" << m_connectState;

    m_connectState = static_cast<int>(state);
    emit connectStateChanged(m_connectState);
    qDebug() << "=== CommViewModel::onStateChanged END ===";
}

void CommViewModel::onRecvData(const QByteArray& data)
{
    // 只有当数据显示页面激活时才更新接收窗口
    if (m_isShowViewActive) {
        QString displayData;
        if (m_hexDisplay) {
            displayData = byteArrayToHexString(data);
        } else {
            displayData = QString::fromUtf8(data);
        }
        m_receiveData += displayData;
        emit receiveDataChanged(m_receiveData);
    }

    // 只有当数据绘图页面激活时才解析数据
    if (m_isPlotViewActive && m_parseEnabled) {
        m_dataParser->parse(data);
    }
}

void CommViewModel::onErrorOccurred(const QString& message)
{
    emit errorOccurred(message);
}

void CommViewModel::onNewAccelData(const AccelerationData& data)
{
    if (m_parseEnabled) {
        m_dataPlotViewModel->addDataPoint(data.timestamp, data.ax, data.ay, data.az, 0, 0, 0, 0, 0, 0);
        emit accelerationDataReady(data);
    }
}

void CommViewModel::onNewGyroData(const GyroscopeData& data)
{
    if (m_parseEnabled) {
        m_dataPlotViewModel->addDataPoint(data.timestamp, 0, 0, 0, data.gx, data.gy, data.gz, 0, 0, 0);
        emit gyroscopeDataReady(data);
    }
}

void CommViewModel::onNewSensorData(const SensorData& data)
{
    if (m_parseEnabled) {
        qDebug() << "[CommViewModel] onNewSensorData: Acc=" << data.Acc[0] << data.Acc[1] << data.Acc[2]
                 << ", Gyro=" << data.Gyro[0] << data.Gyro[1] << data.Gyro[2]
                 << ", Angle=" << data.Angle[0] << data.Angle[1] << data.Angle[2];
        m_dataPlotViewModel->addDataPoint(static_cast<qint64>(data.Timestamp),
            data.Acc[0], data.Acc[1], data.Acc[2],
            data.Gyro[0], data.Gyro[1], data.Gyro[2],
            data.Angle[0], data.Angle[1], data.Angle[2]);
        emit sensorDataReady(data);
    }
}

void CommViewModel::onNewSensorDataPacket(const SensorDataPacket& data)
{
    if (m_parseEnabled) {
        qDebug() << "[CommViewModel] onNewSensorDataPacket: Acc=" << data.Acceleration[0] << data.Acceleration[1] << data.Acceleration[2]
                 << ", Gyro=" << data.AngularVelocity[0] << data.AngularVelocity[1] << data.AngularVelocity[2]
                 << ", Angle=" << data.EulerAngles[0] << data.EulerAngles[1] << data.EulerAngles[2]
                 << ", Temp=" << data.Temperature
                 << ", Quaternion=" << data.Quaternion[0] << data.Quaternion[1] << data.Quaternion[2] << data.Quaternion[3]
                 << ", Magnetometer=" << data.Magnetometer[0] << data.Magnetometer[1] << data.Magnetometer[2];

        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        m_dataPlotViewModel->addDataPoint(timestamp,
            data.Acceleration[0], data.Acceleration[1], data.Acceleration[2],
            data.AngularVelocity[0], data.AngularVelocity[1], data.AngularVelocity[2],
            data.EulerAngles[0], data.EulerAngles[1], data.EulerAngles[2]);
    }
}

void CommViewModel::onNewAllData(const AllDataPacket& data)
{
    if (m_parseEnabled) {
        qDebug() << "[CommViewModel] onNewAllData: Acc=" << data.Acceleration[0] << data.Acceleration[1] << data.Acceleration[2]
                 << ", Gyro=" << data.AngularVelocity[0] << data.AngularVelocity[1] << data.AngularVelocity[2]
                 << ", Angle=" << data.EulerAngles[0] << data.EulerAngles[1] << data.EulerAngles[2];
        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
        m_dataPlotViewModel->addDataPoint(timestamp,
            data.Acceleration[0], data.Acceleration[1], data.Acceleration[2],
            data.AngularVelocity[0], data.AngularVelocity[1], data.AngularVelocity[2],
            data.EulerAngles[0], data.EulerAngles[1], data.EulerAngles[2]);
    }
}

void CommViewModel::onParseError(const QString& error)
{
    m_dataPlotViewModel->onParseError(error);
}

QString CommViewModel::byteArrayToHexString(const QByteArray& data) const
{
    QString hex;
    for (int i = 0; i < data.size(); ++i) {
        hex += QString("%1 ").arg(static_cast<unsigned char>(data[i]), 2, 16, QChar('0')).toUpper();
        if ((i + 1) % 16 == 0) {
            hex += "\n";
        }
    }
    return hex.trimmed();
}

QByteArray CommViewModel::hexStringToByteArray(const QString& hex) const
{
    QByteArray data;
    QString cleanHex = hex;
    cleanHex.remove(QRegularExpression("[^0-9A-Fa-f]"));

    for (int i = 0; i < cleanHex.size(); i += 2) {
        QString byteStr = cleanHex.mid(i, 2);
        bool ok;
        char byte = static_cast<char>(byteStr.toInt(&ok, 16));
        if (ok) {
            data.append(byte);
        }
    }
    return data;
}

SerialBaudRate CommViewModel::baudRateFromInt(int baud) const
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

SerialDataBits CommViewModel::dataBitsFromInt(int bits) const
{
    switch (bits) {
    case 5: return SerialDataBits::D5;
    case 6: return SerialDataBits::D6;
    case 7: return SerialDataBits::D7;
    case 8: return SerialDataBits::D8;
    default: return SerialDataBits::D8;
    }
}

SerialParity CommViewModel::parityFromInt(int index) const
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

SerialStopBits CommViewModel::stopBitsFromInt(int index) const
{
    switch (index) {
    case 0: return SerialStopBits::S1;
    case 1: return SerialStopBits::S1_5;
    case 2: return SerialStopBits::S2;
    default: return SerialStopBits::S1;
    }
}

SerialFlowControl CommViewModel::flowControlFromInt(int index) const
{
    switch (index) {
    case 0: return SerialFlowControl::None;
    case 1: return SerialFlowControl::RTS_CTS;
    case 2: return SerialFlowControl::XON_XOFF;
    default: return SerialFlowControl::None;
    }
}
