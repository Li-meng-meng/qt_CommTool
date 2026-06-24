#include "DataPlotViewModel.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QDateTime>
#include <QDebug>

DataPlotViewModel::DataPlotViewModel(QObject *parent)
    : QObject(parent)
    , m_isPaused(false)
    , m_isViewActive(false)
    , m_startTimestamp(-1)
    , m_maxPoints(5000)
{
}

DataPlotViewModel::~DataPlotViewModel()
{
}

QVariantList DataPlotViewModel::timeValues() const
{
    return m_timeValues;
}

QVariantList DataPlotViewModel::xValues() const
{
    return m_xValues;
}

QVariantList DataPlotViewModel::yValues() const
{
    return m_yValues;
}

QVariantList DataPlotViewModel::zValues() const
{
    return m_zValues;
}

QVariantList DataPlotViewModel::gxValues() const
{
    return m_gxValues;
}

QVariantList DataPlotViewModel::gyValues() const
{
    return m_gyValues;
}

QVariantList DataPlotViewModel::gzValues() const
{
    return m_gzValues;
}

QVariantList DataPlotViewModel::rollValues() const
{
    return m_rollValues;
}

QVariantList DataPlotViewModel::pitchValues() const
{
    return m_pitchValues;
}

QVariantList DataPlotViewModel::yawValues() const
{
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
            emit dataChanged();
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

    if (m_isViewActive) {
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

    for (int i = 0; i < m_timeValues.size(); ++i) {
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
    }

    trimDataLists();

    if (m_isViewActive) {
        emit dataChanged();
    }
}

void DataPlotViewModel::onParseError(const QString& error)
{
    m_errorMessage = error;
    emit errorOccurred(error);
}

void DataPlotViewModel::trimDataLists()
{
    while (m_timeValues.size() > m_maxPoints) {
        m_timeValues.removeFirst();
        m_xValues.removeFirst();
        m_yValues.removeFirst();
        m_zValues.removeFirst();
        m_gxValues.removeFirst();
        m_gyValues.removeFirst();
        m_gzValues.removeFirst();
        m_rollValues.removeFirst();
        m_pitchValues.removeFirst();
        m_yawValues.removeFirst();
    }
}