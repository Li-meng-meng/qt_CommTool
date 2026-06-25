#include "DataPlotViewModel.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QDateTime>
#include <QDebug>
#include <QTimer>

DataPlotViewModel::DataPlotViewModel(QObject *parent)
    : QObject(parent)
    , m_isPaused(false)
    , m_isViewActive(false)
    , m_startTimestamp(-1)
    , m_maxPoints(5000)
    , m_pendingDataCount(0)
{
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(50);
    m_updateTimer->setSingleShot(false);
    connect(m_updateTimer, &QTimer::timeout, this, &DataPlotViewModel::onUpdateTimeout);
}

DataPlotViewModel::~DataPlotViewModel()
{
}

QVariantList DataPlotViewModel::timeValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_timeValues;
}

QVariantList DataPlotViewModel::xValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_xValues;
}

QVariantList DataPlotViewModel::yValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_yValues;
}

QVariantList DataPlotViewModel::zValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_zValues;
}

QVariantList DataPlotViewModel::gxValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_gxValues;
}

QVariantList DataPlotViewModel::gyValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_gyValues;
}

QVariantList DataPlotViewModel::gzValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_gzValues;
}

QVariantList DataPlotViewModel::rollValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_rollValues;
}

QVariantList DataPlotViewModel::pitchValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_pitchValues;
}

QVariantList DataPlotViewModel::yawValues() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_yawValues;
}

bool DataPlotViewModel::isPaused() const
{
    return m_isPaused;
}

bool DataPlotViewModel::isViewActive() const
{
    return m_isViewActive;
}

void DataPlotViewModel::setViewActive(bool active)
{
    if (m_isViewActive != active) {
        m_isViewActive = active;
        emit isViewActiveChanged(m_isViewActive);
        if (m_isViewActive) {
            m_updateTimer->start();
            emit dataChanged();
        } else {
            m_updateTimer->stop();
        }
    }
}

QString DataPlotViewModel::errorMessage() const
{
    return m_errorMessage;
}

void DataPlotViewModel::addDataPoint(qint64 timestamp, float ax, float ay, float az,
                                       float gx, float gy, float gz,
                                       float roll, float pitch, float yaw)
{
    if (m_isPaused) {
        return;
    }

    QMutexLocker locker(&m_dataMutex);

    if (m_startTimestamp < 0) {
        m_startTimestamp = timestamp;
    }

    double relativeTime = (timestamp - m_startTimestamp) / 1000.0;

    m_timeValues.append(relativeTime);
    m_xValues.append(ax);
    m_yValues.append(ay);
    m_zValues.append(az);
    m_gxValues.append(gx);
    m_gyValues.append(gy);
    m_gzValues.append(gz);
    m_rollValues.append(roll);
    m_pitchValues.append(pitch);
    m_yawValues.append(yaw);

    trimDataLists();

    m_pendingDataCount++;
}

void DataPlotViewModel::onUpdateTimeout()
{
    QMutexLocker locker(&m_dataMutex);
    if (m_pendingDataCount > 0) {
        m_pendingDataCount = 0;
        m_timeValues = QVariantList(m_timeValues);
        m_xValues = QVariantList(m_xValues);
        m_yValues = QVariantList(m_yValues);
        m_zValues = QVariantList(m_zValues);
        m_gxValues = QVariantList(m_gxValues);
        m_gyValues = QVariantList(m_gyValues);
        m_gzValues = QVariantList(m_gzValues);
        m_rollValues = QVariantList(m_rollValues);
        m_pitchValues = QVariantList(m_pitchValues);
        m_yawValues = QVariantList(m_yawValues);
        locker.unlock();
        emit dataChanged();
    }
}

void DataPlotViewModel::clearChart()
{
    QMutexLocker locker(&m_dataMutex);

    m_timeValues.clear();
    m_xValues.clear();
    m_yValues.clear();
    m_zValues.clear();
    m_gxValues.clear();
    m_gyValues.clear();
    m_gzValues.clear();
    m_rollValues.clear();
    m_pitchValues.clear();
    m_yawValues.clear();
    m_startTimestamp = -1;
    m_errorMessage.clear();
    m_pendingDataCount = 0;

    emit dataChanged();
    emit errorOccurred(QString());
}

void DataPlotViewModel::setPaused(bool paused)
{
    if (m_isPaused != paused) {
        m_isPaused = paused;
        emit isPausedChanged(m_isPaused);
    }
}

bool DataPlotViewModel::exportChartData(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred(tr("无法打开文件: %1").arg(filePath));
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "Time(s),AX(m/s²),AY(m/s²),AZ(m/s²),GX(°/s),GY(°/s),GZ(°/s),Roll(°),Pitch(°),Yaw(°)\n";

    QMutexLocker locker(&m_dataMutex);
    int count = m_timeValues.size();
    for (int i = 0; i < count; ++i) {
        out << m_timeValues[i].toDouble() << ","
            << m_xValues[i].toFloat() << ","
            << m_yValues[i].toFloat() << ","
            << m_zValues[i].toFloat() << ","
            << m_gxValues[i].toFloat() << ","
            << m_gyValues[i].toFloat() << ","
            << m_gzValues[i].toFloat() << ","
            << m_rollValues[i].toFloat() << ","
            << m_pitchValues[i].toFloat() << ","
            << m_yawValues[i].toFloat() << "\n";
    }

    file.close();
    return true;
}

void DataPlotViewModel::onNewAccelerationData(const AccelerationData& data)
{
    addDataPoint(data.timestamp, data.ax, data.ay, data.az, 0, 0, 0, 0, 0, 0);
}

void DataPlotViewModel::onBatchDataParsed(const QList<AccelerationData>& data)
{
    if (m_isPaused) {
        return;
    }

    QMutexLocker locker(&m_dataMutex);

    int batchCount = 0;
    for (const auto& item : data) {
        if (m_startTimestamp < 0) {
            m_startTimestamp = item.timestamp;
        }

        double relativeTime = (item.timestamp - m_startTimestamp) / 1000.0;

        m_timeValues.append(relativeTime);
        m_xValues.append(item.ax);
        m_yValues.append(item.ay);
        m_zValues.append(item.az);
        m_gxValues.append(0);
        m_gyValues.append(0);
        m_gzValues.append(0);
        m_rollValues.append(0);
        m_pitchValues.append(0);
        m_yawValues.append(0);
        batchCount++;
    }

    trimDataLists();
    m_pendingDataCount += batchCount;
}

void DataPlotViewModel::onParseError(const QString& error)
{
    m_errorMessage = error;
    emit errorOccurred(error);
}

void DataPlotViewModel::trimDataLists()
{
    int excess = m_timeValues.size() - m_maxPoints;
    if (excess > 0) {
        m_timeValues.erase(m_timeValues.begin(), m_timeValues.begin() + excess);
        m_xValues.erase(m_xValues.begin(), m_xValues.begin() + excess);
        m_yValues.erase(m_yValues.begin(), m_yValues.begin() + excess);
        m_zValues.erase(m_zValues.begin(), m_zValues.begin() + excess);
        m_gxValues.erase(m_gxValues.begin(), m_gxValues.begin() + excess);
        m_gyValues.erase(m_gyValues.begin(), m_gyValues.begin() + excess);
        m_gzValues.erase(m_gzValues.begin(), m_gzValues.begin() + excess);
        m_rollValues.erase(m_rollValues.begin(), m_rollValues.begin() + excess);
        m_pitchValues.erase(m_pitchValues.begin(), m_pitchValues.begin() + excess);
        m_yawValues.erase(m_yawValues.begin(), m_yawValues.begin() + excess);
    }
}