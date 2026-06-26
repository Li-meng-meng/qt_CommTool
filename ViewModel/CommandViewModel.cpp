#include "CommandViewModel.h"
#include <QVariantMap>
#include <QDebug>
#include <QClipboard>
#include <QGuiApplication>
#include <QtEndian>
#include <QDateTime>
#include "../Help/crc.h"

CommandViewModel::CommandViewModel(QObject *parent)
    : QObject(parent)
    , m_receiveBufferDirty(false)
    , m_dataTextDirty(false)
    , m_hexdDirty(false)
    , m_flushTimer(new QTimer(this))
{
    connect(m_flushTimer, &QTimer::timeout,
        this, &CommandViewModel::flushReceiveBuffer);
    m_flushTimer->setInterval(2000);
    m_flushTimer->start();

    initCommandGroups();
}

CommandViewModel::~CommandViewModel()
{
}

QVariantList CommandViewModel::commandGroups() const
{
    return m_commandGroups;
}

QVariantMap CommandViewModel::selectedCommand() const
{
    return m_selectedCommand;
}

QString CommandViewModel::generatedHex() const
{
    return m_generatedHex;
}

QString CommandViewModel::receivedHex() const
{
    return m_receivedHex;
}

QString CommandViewModel::receivedDataText() const
{
    return m_receivedDataText;
}

int CommandViewModel::selectedOp() const
{
    return m_selectedOp;
}

int CommandViewModel::selectedData1() const
{
    return m_selectedData1;
}

QVariantList CommandViewModel::chartConfigs() const
{
    return m_chartConfigs;
}

QVariantList CommandViewModel::getChartTimeValues(int chartIndex) const
{
    if (chartIndex < 0 || chartIndex >= m_chartTimeLists.size()) {
        return {};
    }
    return m_chartTimeLists[chartIndex];
}

QVariantList CommandViewModel::getChartSeriesValues(int chartIndex, int seriesIndex) const
{
    if (chartIndex < 0 || chartIndex >= m_chartSeriesLists.size()) {
        return {};
    }
    if (seriesIndex < 0 || seriesIndex >= m_chartSeriesLists[chartIndex].size()) {
        return {};
    }
    return m_chartSeriesLists[chartIndex][seriesIndex];
}

void CommandViewModel::clearChartData()
{
    for (auto& list : m_chartTimeLists) {
        list.clear();
    }
    for (auto& outer : m_chartSeriesLists) {
        for (auto& inner : outer) {
            inner.clear();
        }
    }
    m_chartStartTime = 0;
    m_chartDataVersion++;
    emit chartDataVersionChanged();
    emit chartDataChanged();
}

void CommandViewModel::setChartPaused(bool paused)
{
    if (m_chartPaused != paused) {
        m_chartPaused = paused;
        emit chartPausedChanged(paused);
    }
}

int CommandViewModel::chartDataVersion() const
{
    return m_chartDataVersion;
}

bool CommandViewModel::chartPaused() const
{
    return m_chartPaused;
}

void CommandViewModel::updateChartConfigs()
{
    m_chartConfigs.clear();
    m_chartTimeLists.clear();
    m_chartSeriesLists.clear();

    if (m_selectedCommand.isEmpty()) {
        emit chartConfigsChanged();
        return;
    }

    int cmd = m_selectedCommand.value("cmd").toInt();
    int id = m_selectedCommand.value("subId").toInt();

    if (cmd == 0x00 && id == 0x01) {
        m_chartConfigs.append(QVariantMap{
            {"title", "加速度"},
            {"yAxisLabel", "m/s²"},
            {"yMin", -20},
            {"yMax", 20},
            {"seriesLabels", QStringList{"AX", "AY", "AZ"}}
        });
        m_chartConfigs.append(QVariantMap{
            {"title", "角速度"},
            {"yAxisLabel", "deg/s"},
            {"yMin", -500},
            {"yMax", 500},
            {"seriesLabels", QStringList{"GX", "GY", "GZ"}}
        });
        m_chartConfigs.append(QVariantMap{
            {"title", "姿态角"},
            {"yAxisLabel", "deg"},
            {"yMin", -180},
            {"yMax", 180},
            {"seriesLabels", QStringList{"Roll", "Pitch", "Yaw"}}
        });
        m_chartConfigs.append(QVariantMap{
            {"title", "磁场"},
            {"yAxisLabel", "uT"},
            {"yMin", -100},
            {"yMax", 100},
            {"seriesLabels", QStringList{"MX", "MY", "MZ"}}
        });
    } else if (cmd == 0x00 && id == 0x10) {
        m_chartConfigs.append(QVariantMap{
            {"title", "加速度"},
            {"yAxisLabel", "m/s²"},
            {"yMin", -20},
            {"yMax", 20},
            {"seriesLabels", QStringList{"AX", "AY", "AZ"}}
        });
        m_chartConfigs.append(QVariantMap{
            {"title", "角速度"},
            {"yAxisLabel", "deg/s"},
            {"yMin", -500},
            {"yMax", 500},
            {"seriesLabels", QStringList{"GX", "GY", "GZ"}}
        });
        m_chartConfigs.append(QVariantMap{
            {"title", "姿态角"},
            {"yAxisLabel", "deg"},
            {"yMin", -180},
            {"yMax", 180},
            {"seriesLabels", QStringList{"Roll", "Pitch", "Yaw"}}
        });
    } else if (cmd == 0x01 && id == 0x02) {
        m_chartConfigs.append(QVariantMap{
            {"title", "加速度"},
            {"yAxisLabel", "m/s²"},
            {"yMin", -20},
            {"yMax", 20},
            {"seriesLabels", QStringList{"AX", "AY", "AZ"}}
        });
        m_chartConfigs.append(QVariantMap{
            {"title", "角速度"},
            {"yAxisLabel", "deg/s"},
            {"yMin", -500},
            {"yMax", 500},
            {"seriesLabels", QStringList{"GX", "GY", "GZ"}}
        });
        m_chartConfigs.append(QVariantMap{
            {"title", "姿态角"},
            {"yAxisLabel", "deg"},
            {"yMin", -180},
            {"yMax", 180},
            {"seriesLabels", QStringList{"Roll", "Pitch", "Yaw"}}
        });
        m_chartConfigs.append(QVariantMap{
            {"title", "磁场"},
            {"yAxisLabel", "uT"},
            {"yMin", -100},
            {"yMax", 100},
            {"seriesLabels", QStringList{"MX", "MY", "MZ"}}
        });
    }

    int chartCount = m_chartConfigs.size();
    m_chartTimeLists.resize(chartCount);
    m_chartSeriesLists.resize(chartCount);
    for (int i = 0; i < chartCount; ++i) {
        QVariantMap cfg = m_chartConfigs[i].toMap();
        int seriesCount = cfg.value("seriesLabels").toStringList().size();
        m_chartSeriesLists[i].resize(seriesCount);
    }

    m_chartStartTime = 0;
    m_chartDataVersion++;
    emit chartDataVersionChanged();
    emit chartConfigsChanged();
    emit chartDataChanged();
}

void CommandViewModel::addChartValue(int chartIndex, int seriesIndex, float value, float timeOffset)
{
    Q_UNUSED(timeOffset);
    if (m_chartPaused) return;
    if (chartIndex < 0 || chartIndex >= m_chartTimeLists.size()) return;
    if (seriesIndex < 0 || seriesIndex >= m_chartSeriesLists[chartIndex].size()) return;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (m_chartStartTime == 0) {
        m_chartStartTime = now;
    }
    float t = (now - m_chartStartTime) / 1000.0f;

    if (seriesIndex == 0) {
        m_chartTimeLists[chartIndex].append(t);
        if (m_chartTimeLists[chartIndex].size() > m_maxChartPoints) {
            m_chartTimeLists[chartIndex].removeFirst();
        }
    }

    m_chartSeriesLists[chartIndex][seriesIndex].append(value);
    if (m_chartSeriesLists[chartIndex][seriesIndex].size() > m_maxChartPoints) {
        m_chartSeriesLists[chartIndex][seriesIndex].removeFirst();
    }
}

void CommandViewModel::generateHex()
{
    if (m_selectedCommand.isEmpty()) {
        m_generatedHex = "";
        emit generatedHexChanged();
        return;
    }

    quint8 cmd = m_selectedCommand["cmd"].toUInt();
    quint8 id = m_selectedCommand["subId"].toUInt();
    quint8 op = static_cast<quint8>(m_selectedOp);

    QByteArray payload = buildPayload(cmd, id);

    send_user_head_t head;
    head.sof = 0xAA;
    head.sof1 = 0x55;
    head.cmd = cmd;
    head.id = id;
    head.op = op;
    ushort dataLen = static_cast<ushort>(7 + payload.size() + 2);
    head.v_len = dataLen;

    QByteArray headerBytes(reinterpret_cast<const char*>(&head), sizeof(send_user_head_t));

    QByteArray dataWithoutCrc = headerBytes + payload;

    quint16 calculatedCrc16 = CRC::CRC16(
        reinterpret_cast<const quint8*>(dataWithoutCrc.constData()),
        static_cast<quint16>(dataWithoutCrc.size())
    );

    QByteArray crcBytes(2, 0);
    crcBytes[0] = static_cast<char>(calculatedCrc16 & 0xFF);
    crcBytes[1] = static_cast<char>((calculatedCrc16 >> 8) & 0xFF);

    QByteArray finalPacket = dataWithoutCrc + crcBytes;

    QString hex;
    for (int i = 0; i < finalPacket.size(); ++i) {
        hex += QString("%1 ").arg(static_cast<quint8>(finalPacket[i]), 2, 16, QChar('0')).toUpper();
    }
    hex = hex.trimmed();

    m_generatedHex = hex;
    emit generatedHexChanged();
}

QByteArray CommandViewModel::buildPayload(quint8 cmd, quint8 id)
{
    switch (cmd) {
    case 0:
        switch (id) {
        case 1:
        case 2:
        case 3:
        case 9:
        case 16:
            return QByteArray();
        case 4:
        case 6:
        case 7:
        case 8: {
            QByteArray payload(1, 0);
            payload[0] = static_cast<char>(m_selectedData1 & 0xFF);
            return payload;
        }
        case 5: {
            QByteArray payload(16, 0);
            QString strValue = QString::number(m_selectedData1);
            for (int i = 0; i < strValue.size() && i < 16; ++i) {
                payload[i] = static_cast<char>(strValue.at(i).unicode());
            }
            return payload;
        }
        default:
            return QByteArray();
        }
    case 1:
        switch (id) {
        case 2:
            return QByteArray();
        case 1:
        case 3: {
            QByteArray payload(1, 0);
            payload[0] = static_cast<char>(m_selectedData1 & 0xFF);
            return payload;
        }
        default:
            return QByteArray();
        }
    case 2:
        switch (id) {
        case 2:
            return QByteArray();
        case 1:
        case 3: {
            QByteArray payload(1, 0);
            payload[0] = static_cast<char>(m_selectedData1 & 0xFF);
            return payload;
        }
        default:
            return QByteArray();
        }
    case 3:
        switch (id) {
        default:
            return QByteArray();
        }
    default:
        return QByteArray();
    }
}

void CommandViewModel::copyHex()
{
    if (!m_generatedHex.isEmpty()) {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(m_generatedHex);
        emit hexCopied();
    }
}

void CommandViewModel::sendCommand(quint8 cmd, quint8 id)
{
    quint8 op = static_cast<quint8>(m_selectedOp);

    QByteArray payload = buildPayload(cmd, id);

    send_user_head_t head;
    head.sof = 0xAA;
    head.sof1 = 0x55;
    head.cmd = cmd;
    head.id = id;
    head.op = op;
    ushort dataLen = static_cast<ushort>(7 + payload.size() + 2);
    head.v_len = dataLen;

    QByteArray headerBytes(reinterpret_cast<const char*>(&head), sizeof(send_user_head_t));

    QByteArray dataWithoutCrc = headerBytes + payload;

    quint16 calculatedCrc16 = CRC::CRC16(
        reinterpret_cast<const quint8*>(dataWithoutCrc.constData()),
        static_cast<quint16>(dataWithoutCrc.size())
    );

    QByteArray crcBytes(2, 0);
    crcBytes[0] = static_cast<char>(calculatedCrc16 & 0xFF);
    crcBytes[1] = static_cast<char>((calculatedCrc16 >> 8) & 0xFF);

    QByteArray finalPacket = dataWithoutCrc + crcBytes;

    emit sendDataRequested(finalPacket);
    emit commandSent(cmd, id);

    qDebug() << "[CommandViewModel] Sent command: cmd=" << QString::number(cmd, 16)
             << ", id=" << QString::number(id, 16)
             << ", HEX=" << finalPacket.toHex(' ').toUpper();
}

void CommandViewModel::setSelectedCommand(const QVariantMap& cmd)
{
    m_selectedCommand = cmd;
    emit selectedCommandChanged();

    m_receivedDataText.clear();
    m_pendingDataText.clear();
    m_dataTextDirty = false;
    emit receivedDataTextChanged();

    m_receivedHex.clear();
    m_pendingHex.clear();
    m_hexdDirty = false;
    emit receivedHexChanged();

    updateChartConfigs();
    generateHex();
}

void CommandViewModel::setSelectedOp(int op)
{
    m_selectedOp = op;
    emit selectedOpChanged();
    generateHex();
}

void CommandViewModel::setSelectedData1(int data1)
{
    m_selectedData1 = data1;
    emit selectedData1Changed();
    generateHex();
}

void CommandViewModel::initCommandGroups()
{
    QVariantList group0;
    group0.append(QVariantMap{ {"id", 1}, {"label", "传感器数据推送"}, {"cmd", 0}, {"subId", 1}, {"description", "查询传感器数据"} });
    group0.append(QVariantMap{ {"id", 2}, {"label", "设备状态数据推送"}, {"cmd", 0}, {"subId", 2}, {"description", "查询设备状态"} });
    group0.append(QVariantMap{ {"id", 3}, {"label", "版本查询"}, {"cmd", 0}, {"subId", 3}, {"description", "查询版本信息"} });
    group0.append(QVariantMap{ {"id", 4}, {"label", "传感器数据推送频率设置与查询"}, {"cmd", 0}, {"subId", 4}, {"description", "设置或查询推送频率"} });
    group0.append(QVariantMap{ {"id", 5}, {"label", "修改蓝牙名称"}, {"cmd", 0}, {"subId", 5}, {"description", "修改蓝牙名称"} });
    group0.append(QVariantMap{ {"id", 6}, {"label", "算法模式查询与设置"}, {"cmd", 0}, {"subId", 6}, {"description", "查询或设置算法模式"} });
    group0.append(QVariantMap{ {"id", 7}, {"label", "用户数据推送配置与查询"}, {"cmd", 0}, {"subId", 7}, {"description", "配置用户数据推送"} });
    group0.append(QVariantMap{ {"id", 8}, {"label", "地磁开关"}, {"cmd", 0}, {"subId", 8}, {"description", "地磁开关状态"} });
    group0.append(QVariantMap{ {"id", 9}, {"label", "地磁数据解析"}, {"cmd", 0}, {"subId", 9}, {"description", "地磁校准数据"} });
    group0.append(QVariantMap{ {"id", 16}, {"label", "传感器推送数据2"}, {"cmd", 0}, {"subId", 16}, {"description", "传感器原始数据推送"} });

    QVariantList group1;
    group1.append(QVariantMap{ {"id", 1}, {"label", "A值查询与设置"}, {"cmd", 1}, {"subId", 1}, {"description", "查询或设置A值"} });
    group1.append(QVariantMap{ {"id", 2}, {"label", "设备状态与传感器数据"}, {"cmd", 1}, {"subId", 2}, {"description", "获取全部传感器数据"} });
    group1.append(QVariantMap{ {"id", 3}, {"label", "电池电量"}, {"cmd", 1}, {"subId", 3}, {"description", "查询电池电量"} });

    m_commandGroups.append(QVariantMap{ {"name", "基础设置"}, {"cmd", 0}, {"commands", group0} });
    m_commandGroups.append(QVariantMap{ {"name", "其他数据获取"}, {"cmd", 1}, {"commands", group1} });
}

void CommandViewModel::appendReceivedData(const QByteArray& data)
{
    Q_UNUSED(data);
}

void CommandViewModel::flushReceiveBuffer()
{
    if (m_dataTextDirty) {
        m_receivedDataText = m_pendingDataText;
        m_dataTextDirty = false;
        emit receivedDataTextChanged();
    }

    if (m_hexdDirty) {
        m_receivedHex = m_pendingHex;
        m_hexdDirty = false;
        emit receivedHexChanged();
    }
}

void CommandViewModel::clearReceivedHex()
{
    m_receivedHex.clear();
    m_pendingHex.clear();
    m_hexdDirty = false;
    emit receivedHexChanged();
}

void CommandViewModel::handleReceivedData(quint8 cmd, quint8 id, const QByteArray& data)
{
    if (m_selectedCommand.isEmpty()) {
        return;
    }

    int selectedCmd = m_selectedCommand.value("cmd").toInt();
    int selectedId = m_selectedCommand.value("subId").toInt();

    if (cmd != selectedCmd || id != selectedId) {
        return;
    }

    QString text;
    text += QString("--- 收到命令 Cmd=%1, Id=%2 ---\n").arg(cmd).arg(id);

    switch (cmd) {
    case 0x00:
        switch (id) {
        case 0x01: {
            SensorDataPacket imuData = CommImpl::ByteArrayToStructure<SensorDataPacket>(data);
            text += "【类型】: IMU 传感器数据\n";
            text += QString("频率: %1 Hz\n").arg(static_cast<double>(imuData.PushFrequency), 0, 'F', 2);
            text += QString("时间: %1-%2-%3 %4:%5:%6\n")
                       .arg(imuData.Year)
                       .arg(imuData.Month, 2, 10, QChar('0'))
                       .arg(imuData.Day, 2, 10, QChar('0'))
                       .arg(imuData.Hour, 2, 10, QChar('0'))
                       .arg(imuData.Minute, 2, 10, QChar('0'))
                       .arg(imuData.Second, 2, 10, QChar('0'));
            text += QString("加速度: [%1, %2, %3, %4] m/s²\n")
                       .arg(imuData.Acceleration[0], 0, 'F', 3)
                       .arg(imuData.Acceleration[1], 0, 'F', 3)
                       .arg(imuData.Acceleration[2], 0, 'F', 3)
                       .arg(imuData.Acceleration[3], 0, 'F', 3);
            text += QString("角速度: [%1, %2, %3, %4] deg/s\n")
                       .arg(imuData.AngularVelocity[0], 0, 'F', 3)
                       .arg(imuData.AngularVelocity[1], 0, 'F', 3)
                       .arg(imuData.AngularVelocity[2], 0, 'F', 3)
                       .arg(imuData.AngularVelocity[3], 0, 'F', 3);
            text += QString("温度:   %1 °C\n").arg(imuData.Temperature, 0, 'F', 2);
            text += QString("姿态角: [%1, %2, %3] deg\n")
                       .arg(imuData.EulerAngles[0], 0, 'F', 2)
                       .arg(imuData.EulerAngles[1], 0, 'F', 2)
                       .arg(imuData.EulerAngles[2], 0, 'F', 2);
            text += QString("四元数: [%1, %2, %3, %4]\n")
                       .arg(imuData.Quaternion[0], 0, 'F', 4)
                       .arg(imuData.Quaternion[1], 0, 'F', 4)
                       .arg(imuData.Quaternion[2], 0, 'F', 4)
                       .arg(imuData.Quaternion[3], 0, 'F', 4);
            text += QString("磁场:   [%1, %2, %3] uT\n")
                       .arg(imuData.Magnetometer[0], 0, 'F', 2)
                       .arg(imuData.Magnetometer[1], 0, 'F', 2)
                       .arg(imuData.Magnetometer[2], 0, 'F', 2);
            text += QString("CRC16:  0x%1\n").arg(imuData.Crc16, 4, 16, QChar('0')).toUpper();

            addChartValue(0, 0, imuData.Acceleration[0]);
            addChartValue(0, 1, imuData.Acceleration[1]);
            addChartValue(0, 2, imuData.Acceleration[2]);
            addChartValue(1, 0, imuData.AngularVelocity[0]);
            addChartValue(1, 1, imuData.AngularVelocity[1]);
            addChartValue(1, 2, imuData.AngularVelocity[2]);
            addChartValue(2, 0, imuData.EulerAngles[0]);
            addChartValue(2, 1, imuData.EulerAngles[1]);
            addChartValue(2, 2, imuData.EulerAngles[2]);
            addChartValue(3, 0, imuData.Magnetometer[0]);
            addChartValue(3, 1, imuData.Magnetometer[1]);
            addChartValue(3, 2, imuData.Magnetometer[2]);
            m_chartDataVersion++;
            emit chartDataVersionChanged();
            emit chartDataChanged();
            break;
        }
        case 0x02: {
            StatusPacket statusPacket = CommImpl::ByteArrayToStructure<StatusPacket>(data);
            text += "【类型】: 设备状态信息\n";
            text += QString("用户A值: %1\n").arg(statusPacket.UserA);
            text += QString("电量: %1\n").arg(statusPacket.BatteryLevel);
            text += QString("CRC16:    0x%1\n").arg(statusPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x03: {
            VisonPacket visonPacket = CommImpl::ByteArrayToStructure<VisonPacket>(data);
            text += "【类型】: 版本信息\n";
            text += QString("版本号: %1\n").arg(visonPacket.vison);
            text += QString("CRC16:    0x%1\n").arg(visonPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x04: {
            HzPacket hzPacket = CommImpl::ByteArrayToStructure<HzPacket>(data);
            text += "【类型】: 数据推送频率信息\n";
            text += QString("Hz: %1\n").arg(hzPacket.hz);
            text += QString("CRC16:    0x%1\n").arg(hzPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x05: {
            SnPacket snPacket = CommImpl::ByteArrayToStructure<SnPacket>(data);
            text += "【类型】: SN设置状态信息\n";
            text += QString("占位数据: %1\n").arg(snPacket.data);
            text += QString("CRC16:    0x%1\n").arg(snPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x06: {
            AlgorithmPacket algorithmPacket = CommImpl::ByteArrayToStructure<AlgorithmPacket>(data);
            text += "【类型】: 算法信息\n";
            text += QString("算法模式: %1\n").arg(algorithmPacket.algorithm);
            text += QString("CRC16:    0x%1\n").arg(algorithmPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x07: {
            APacket aPacket = CommImpl::ByteArrayToStructure<APacket>(data);
            text += "【类型】: A值信息\n";
            text += QString("A值状态: %1\n").arg(aPacket.a);
            text += QString("CRC16:    0x%1\n").arg(aPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x08: {
            APacket aPacket = CommImpl::ByteArrayToStructure<APacket>(data);
            text += "【类型】: A值信息\n";
            text += QString("开关状态值: %1\n").arg(aPacket.a);
            text += QString("CRC16:    0x%1\n").arg(aPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x09: {
            CalibrationData calibrationData = CommImpl::ByteArrayToStructure<CalibrationData>(data);
            text += "【类型】: 校准数据\n";
            text += QString("校准进度: %1\n").arg(calibrationData.Progress, 0, 'F', 2);
            text += QString("校准是否有效: %1\n").arg(calibrationData.IsValid);
            text += QString("涵盖极值方向数量: %1\n").arg(calibrationData.DirCount);
            text += QString("有效样本数量: %1\n").arg(calibrationData.SampleCount);
            text += QString("CRC16:    0x%1\n").arg(calibrationData.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x10: {
            ::SensorData sensorData = CommImpl::ByteArrayToStructure<::SensorData>(data);
            text += "【类型】: 传感器推送数据 (0x0010)\n";
            text += QString("时间戳(us): %1\n").arg(sensorData.Timestamp);
            text += QString("加速度 X: %1 m/s²\n").arg(sensorData.Acc[0], 0, 'F', 2);
            text += QString("加速度 Y: %1 m/s²\n").arg(sensorData.Acc[1], 0, 'F', 2);
            text += QString("加速度 Z: %1 m/s²\n").arg(sensorData.Acc[2], 0, 'F', 2);
            text += QString("角速度 X: %1 °/S\n").arg(sensorData.Gyro[0], 0, 'F', 2);
            text += QString("角速度 Y: %1 °/S\n").arg(sensorData.Gyro[1], 0, 'F', 2);
            text += QString("角速度 Z: %1 °/S\n").arg(sensorData.Gyro[2], 0, 'F', 2);
            text += QString("姿态 Roll: %1 °\n").arg(sensorData.Angle[0], 0, 'F', 2);
            text += QString("姿态 Pitch: %1 °\n").arg(sensorData.Angle[1], 0, 'F', 2);
            text += QString("姿态 Yaw:   %1 °\n").arg(sensorData.Angle[2], 0, 'F', 2);
            text += QString("CRC16: 0x%1\n").arg(sensorData.Crc16, 4, 16, QChar('0')).toUpper();

            addChartValue(0, 0, sensorData.Acc[0]);
            addChartValue(0, 1, sensorData.Acc[1]);
            addChartValue(0, 2, sensorData.Acc[2]);
            addChartValue(1, 0, sensorData.Gyro[0]);
            addChartValue(1, 1, sensorData.Gyro[1]);
            addChartValue(1, 2, sensorData.Gyro[2]);
            addChartValue(2, 0, sensorData.Angle[0]);
            addChartValue(2, 1, sensorData.Angle[1]);
            addChartValue(2, 2, sensorData.Angle[2]);
            m_chartDataVersion++;
            emit chartDataVersionChanged();
            emit chartDataChanged();
            break;
        }
        default:
            return;
        }
        break;
    case 0x01:
        switch (id) {
        case 0x01: {
            APacket aPacket = CommImpl::ByteArrayToStructure<APacket>(data);
            text += "【类型】: A值信息\n";
            text += QString("用户A值: %1\n").arg(aPacket.a);
            text += QString("CRC16:    0x%1\n").arg(aPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        case 0x02: {
            AllDataPacket imuData = CommImpl::ByteArrayToStructure<AllDataPacket>(data);
            text += "【类型】: IMU 传感器数据\n";
            text += QString("频率: %1 Hz\n").arg(static_cast<double>(imuData.PushFrequency), 0, 'F', 2);
            text += QString("加速度: [%1, %2, %3, %4] m/s²\n")
                       .arg(imuData.Acceleration[0], 0, 'F', 3)
                       .arg(imuData.Acceleration[1], 0, 'F', 3)
                       .arg(imuData.Acceleration[2], 0, 'F', 3)
                       .arg(imuData.Acceleration[3], 0, 'F', 3);
            text += QString("角速度: [%1, %2, %3, %4] deg/s\n")
                       .arg(imuData.AngularVelocity[0], 0, 'F', 3)
                       .arg(imuData.AngularVelocity[1], 0, 'F', 3)
                       .arg(imuData.AngularVelocity[2], 0, 'F', 3)
                       .arg(imuData.AngularVelocity[3], 0, 'F', 3);
            text += QString("温度:   %1 °C\n").arg(imuData.Temperature, 0, 'F', 2);
            text += QString("姿态角: [%1, %2, %3] deg\n")
                       .arg(imuData.EulerAngles[0], 0, 'F', 2)
                       .arg(imuData.EulerAngles[1], 0, 'F', 2)
                       .arg(imuData.EulerAngles[2], 0, 'F', 2);
            text += QString("四元数: [%1, %2, %3, %4]\n")
                       .arg(imuData.Quaternion[0], 0, 'F', 4)
                       .arg(imuData.Quaternion[1], 0, 'F', 4)
                       .arg(imuData.Quaternion[2], 0, 'F', 4)
                       .arg(imuData.Quaternion[3], 0, 'F', 4);
            text += QString("磁场:   [%1, %2, %3] uT\n")
                       .arg(imuData.Magnetometer[0], 0, 'F', 2)
                       .arg(imuData.Magnetometer[1], 0, 'F', 2)
                       .arg(imuData.Magnetometer[2], 0, 'F', 2);
            text += QString("用户A值: %1\n").arg(imuData.UserA);
            text += QString("电量: %1\n").arg(imuData.BatteryLevel);
            text += QString("CRC16:  0x%1\n").arg(imuData.Crc16, 4, 16, QChar('0')).toUpper();

            addChartValue(0, 0, imuData.Acceleration[0]);
            addChartValue(0, 1, imuData.Acceleration[1]);
            addChartValue(0, 2, imuData.Acceleration[2]);
            addChartValue(1, 0, imuData.AngularVelocity[0]);
            addChartValue(1, 1, imuData.AngularVelocity[1]);
            addChartValue(1, 2, imuData.AngularVelocity[2]);
            addChartValue(2, 0, imuData.EulerAngles[0]);
            addChartValue(2, 1, imuData.EulerAngles[1]);
            addChartValue(2, 2, imuData.EulerAngles[2]);
            addChartValue(3, 0, imuData.Magnetometer[0]);
            addChartValue(3, 1, imuData.Magnetometer[1]);
            addChartValue(3, 2, imuData.Magnetometer[2]);
            m_chartDataVersion++;
            emit chartDataVersionChanged();
            emit chartDataChanged();
            break;
        }
        case 0x03: {
            PowerPacket powerPacket = CommImpl::ByteArrayToStructure<PowerPacket>(data);
            text += "【类型】: A值信息\n";
            text += QString("用户A值: %1\n").arg(powerPacket.power);
            text += QString("CRC16:    0x%1\n").arg(powerPacket.Crc16, 4, 16, QChar('0')).toUpper();
            break;
        }
        default:
            return;
        }
        break;
    default:
        return;
    }

    m_pendingDataText = text;
    m_dataTextDirty = true;

    static const char hexDigits[] = "0123456789ABCDEF";
    QString hexStr;
    hexStr.reserve(data.size() * 3);
    for (int i = 0; i < data.size(); ++i) {
        if (i > 0) hexStr += ' ';
        unsigned char byte = static_cast<unsigned char>(data[i]);
        hexStr += hexDigits[byte >> 4];
        hexStr += hexDigits[byte & 0x0F];
    }

    m_pendingHex = hexStr;
    m_hexdDirty = true;
}
