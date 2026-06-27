#include "CommViewModel.h"
#include "Comm/SerialCommHandler.h"
#include "Comm/BluetoothCommHandler.h"
#include <QRegularExpression>
#include <QDateTime>
#include <QDebug>
#include <QTimer>

CommViewModel::CommViewModel(QObject *parent)
    : QObject(parent)
    , m_hexDisplay(true)
    , m_hexSend(true)
    , m_parseEnabled(true)
    , m_isPlotViewActive(false)
    , m_isShowViewActive(true)
    , m_receiveBufferDirty(false)
    , m_receiveFlushTimer(new QTimer(this))
    , m_commManager(new CommManager(this))
    , m_dataParser(new DataParser(this))
    , m_dataPlotViewModel(new DataPlotViewModel(this))
    , m_commandViewModel(new CommandViewModel(this))
{
    m_commManager->registerHandler(new SerialCommHandler(this));
    m_commManager->registerHandler(new BluetoothCommHandler(this));

    connect(m_receiveFlushTimer, &QTimer::timeout,
        this, &CommViewModel::flushReceiveBuffer);
    m_receiveFlushTimer->setInterval(100);
    m_receiveFlushTimer->start();

    qDebug() << "[CommViewModel] connecting CommManager signals";
    connect(m_commManager, &CommManager::connectStateChanged,
        this, &CommViewModel::onStateChanged);
    connect(m_commManager, &CommManager::dataReceived,
        this, &CommViewModel::onRecvData);
    connect(m_commManager, &CommManager::errorOccurred,
        this, &CommViewModel::onErrorOccurred);
    qDebug() << "[CommViewModel] CommManager signals connected";
    qDebug() << "[CommViewModel] currentHandler registered:" << m_commManager->getAvailableTypes();

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

    connect(m_commandViewModel, &CommandViewModel::sendDataRequested,
        this, [this](const QByteArray& data) {
            m_commManager->sendData(data);
        });

    connect(m_dataParser, &DataParser::newReceivedData,
        m_commandViewModel, &CommandViewModel::handleReceivedData);

    m_commManager->setCurrentType("serial");
}

CommViewModel::~CommViewModel()
{
    qDebug() << "=== CommViewModel destructor START ===";
    m_commManager->closePort();
    qDebug() << "=== CommViewModel destructor END ===";
}

int CommViewModel::getConnectState() const
{
    return m_commManager->getConnectState();
}

QString CommViewModel::getReceiveData() const
{
    return m_receiveData;
}

QString CommViewModel::getSendHistory() const
{
    return m_sendHistory;
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
        if (m_dataPlotViewModel) {
            m_dataPlotViewModel->setViewActive(active);
        }
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
        if (active && m_receiveBufferDirty) {
            flushReceiveBuffer();
        }
    }
}

DataPlotViewModel* CommViewModel::getDataPlotViewModel() const
{
    return m_dataPlotViewModel;
}

CommandViewModel* CommViewModel::getCommandViewModel() const
{
    return m_commandViewModel;
}

CommManager* CommViewModel::getCommManager() const
{
    return m_commManager;
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

    qDebug() << "  Calling m_commManager->sendData()...";
    bool ret = m_commManager->sendData(sendData);
    qDebug() << "  m_commManager->sendData() returned:" << ret;

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

void CommViewModel::clearReceiveData()
{
    m_receiveData.clear();
    m_receiveBuffer.clear();
    m_receiveBufferDirty = false;
    emit receiveDataChanged(m_receiveData);
}

void CommViewModel::clearSendHistory()
{
    m_sendHistory.clear();
    emit sendHistoryChanged(m_sendHistory);
}

void CommViewModel::onStateChanged(int state)
{
    qDebug() << "=== CommViewModel::onStateChanged START ===";
    qDebug() << "  New state:" << state;
    qDebug() << "  Old state:" << m_connectState;

    m_connectState = state;
    emit connectStateChanged(m_connectState);
    qDebug() << "=== CommViewModel::onStateChanged END ===";
}

void CommViewModel::onRecvData(const QByteArray& data)
{
    qDebug() << "[CommViewModel] onRecvData, size:" << data.size() 
             << ", hex:" << data.toHex().left(32)
             << ", parseEnabled:" << m_parseEnabled;

    if (m_isShowViewActive) {
        QString displayData;
        if (m_hexDisplay) {
            displayData = byteArrayToHexString(data);
        } else {
            displayData = QString::fromUtf8(data);
        }
        m_receiveBuffer += displayData;
        m_receiveBufferDirty = true;
    }

    if (m_parseEnabled) {
        m_dataParser->parse(data);
    }
}

void CommViewModel::flushReceiveBuffer()
{
    if (!m_receiveBufferDirty || !m_isShowViewActive) {
        return;
    }

    const int maxChars = 50000;
    if (m_receiveBuffer.size() > maxChars) {
        m_receiveBuffer = m_receiveBuffer.right(maxChars);
    }

    m_receiveData = m_receiveBuffer;
    m_receiveBufferDirty = false;
    emit receiveDataChanged(m_receiveData);
}

void CommViewModel::onErrorOccurred(const QString& message)
{
    emit errorOccurred(message);
}

void CommViewModel::onNewAccelData(const AccelerationData& data)
{
    if (m_parseEnabled) {
        m_dataPlotViewModel->addDataPoint(data.timestamp, data.ax, data.ay, data.az, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        emit accelerationDataReady(data);
    }
}

void CommViewModel::onNewGyroData(const GyroscopeData& data)
{
    if (m_parseEnabled) {
        m_dataPlotViewModel->addDataPoint(data.timestamp, 0, 0, 0, data.gx, data.gy, data.gz, 0, 0, 0, 0, 0, 0);
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
            data.Angle[0], data.Angle[1], data.Angle[2],
            0, 0, 0);
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
            data.EulerAngles[0], data.EulerAngles[1], data.EulerAngles[2],
            data.Magnetometer[0], data.Magnetometer[1], data.Magnetometer[2]);
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
            data.EulerAngles[0], data.EulerAngles[1], data.EulerAngles[2],
            0, 0, 0);
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
