#include "DataParser.h"
#include <QtEndian>
#include <cstring>
#include <QDateTime>

DataParser* DataParser::s_instance = nullptr;

DataParser::DataParser(QObject *parent)
    : QObject(parent), m_commImpl(new CommImpl(this))
{
    s_instance = this;
    connect(m_commImpl, &CommImpl::frameParsed, this, &DataParser::onFrameParsed);
    connect(m_commImpl, &CommImpl::parseError, this, &DataParser::parseError);
}

DataParser::~DataParser()
{
    s_instance = nullptr;
}

void DataParser::parse(const QByteArray& rawData)
{
    m_commImpl->enqueue(rawData);
}

void DataParser::clear()
{
    m_commImpl->clear();
}

void DataParser::onFrameParsed(const QByteArray& frame)
{
    user_unpack_data_fun(frame, static_cast<quint16>(frame.size()));
}

void DataParser::user_unpack_data_fun(const QByteArray& buf, quint16 len)
{
    if (len < sizeof(user_head_t)) {
        emit parseError(tr("帧长度不足"));
        return;
    }

    user_head_t user_head_temp = CommImpl::ByteArrayToStructure<user_head_t>(buf);

    for (int i = 0; i < user_comm_callbak_count; ++i) {
        if ((user_head_temp.cmd == user_comm_callbak[i].cmd) &&
            (user_head_temp.id == user_comm_callbak[i].id)) {
            user_comm_callbak[i].fun(buf, len);
            emit s_instance->newReceivedData(user_head_temp.cmd, user_head_temp.id, buf);
            return;
        }
    }
}

void DataParser::cmd0_id1_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(SensorDataPacket))) {
        emit s_instance->parseError(tr("SensorDataPacket数据长度不足"));
        return;
    }
    SensorDataPacket imuData = CommImpl::ByteArrayToStructure<SensorDataPacket>(buf);

    emit s_instance->newSensorDataPacket(imuData);
}

void DataParser::cmd0_id2_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(StatusPacket))) {
        emit s_instance->parseError(tr("StatusPacket数据长度不足"));
        return;
    }
    StatusPacket statusPacket = CommImpl::ByteArrayToStructure<StatusPacket>(buf);
    emit s_instance->newStatusData(statusPacket);
}

void DataParser::cmd0_id3_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(VisonPacket))) {
        emit s_instance->parseError(tr("VisonPacket数据长度不足"));
        return;
    }
    VisonPacket visonPacket = CommImpl::ByteArrayToStructure<VisonPacket>(buf);
    Q_UNUSED(visonPacket);
}

void DataParser::cmd0_id4_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(HzPacket))) {
        emit s_instance->parseError(tr("HzPacket数据长度不足"));
        return;
    }
    HzPacket hzPacket = CommImpl::ByteArrayToStructure<HzPacket>(buf);
    Q_UNUSED(hzPacket);
}

void DataParser::cmd0_id5_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(SnPacket))) {
        emit s_instance->parseError(tr("SnPacket数据长度不足"));
        return;
    }
    SnPacket snPacket = CommImpl::ByteArrayToStructure<SnPacket>(buf);
    Q_UNUSED(snPacket);
}

void DataParser::cmd0_id6_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(AlgorithmPacket))) {
        emit s_instance->parseError(tr("AlgorithmPacket数据长度不足"));
        return;
    }
    AlgorithmPacket algorithmPacket = CommImpl::ByteArrayToStructure<AlgorithmPacket>(buf);
    Q_UNUSED(algorithmPacket);
}

void DataParser::cmd0_id7_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(APacket))) {
        emit s_instance->parseError(tr("APacket数据长度不足"));
        return;
    }
    APacket aPacket = CommImpl::ByteArrayToStructure<APacket>(buf);
    Q_UNUSED(aPacket);
}

void DataParser::cmd0_id8_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(APacket))) {
        emit s_instance->parseError(tr("APacket数据长度不足"));
        return;
    }
    APacket aPacket = CommImpl::ByteArrayToStructure<APacket>(buf);
    Q_UNUSED(aPacket);
}

void DataParser::cmd0_id9_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(CalibrationData))) {
        emit s_instance->parseError(tr("CalibrationData数据长度不足"));
        return;
    }
    CalibrationData calibrationData = CommImpl::ByteArrayToStructure<CalibrationData>(buf);
    Q_UNUSED(calibrationData);
}

void DataParser::cmd0_id10_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(::SensorData))) {
        emit s_instance->parseError(tr("SensorData数据长度不足"));
        return;
    }
    ::SensorData sensorData = CommImpl::ByteArrayToStructure<::SensorData>(buf);
    emit s_instance->newSensorData(sensorData);
}

void DataParser::cmd1_id1_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(APacket))) {
        emit s_instance->parseError(tr("APacket数据长度不足"));
        return;
    }
    APacket aPacket = CommImpl::ByteArrayToStructure<APacket>(buf);
    Q_UNUSED(aPacket);
}

void DataParser::cmd1_id2_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    qDebug() << "[DataParser] cmd1_id2_get_data called, len=" << len;
    if (len < static_cast<quint16>(sizeof(AllDataPacket))) {
        emit s_instance->parseError(tr("AllDataPacket数据长度不足"));
        return;
    }
    AllDataPacket imuData = CommImpl::ByteArrayToStructure<AllDataPacket>(buf);
    qDebug() << "[DataParser] AllDataPacket: Acc=" << imuData.Acceleration[0] << imuData.Acceleration[1] << imuData.Acceleration[2]
             << ", Gyro=" << imuData.AngularVelocity[0] << imuData.AngularVelocity[1] << imuData.AngularVelocity[2];
    emit s_instance->newAllData(imuData);
}

void DataParser::cmd1_id3_get_data(const QByteArray& buf, quint16 len)
{
    if (!s_instance) return;
    if (len < static_cast<quint16>(sizeof(PowerPacket))) {
        emit s_instance->parseError(tr("PowerPacket数据长度不足"));
        return;
    }
    PowerPacket powerPacket = CommImpl::ByteArrayToStructure<PowerPacket>(buf);
    Q_UNUSED(powerPacket);
}

void DataParser::cmd2_id1_get_data(const QByteArray& buf, quint16 len)
{
    Q_UNUSED(buf);
    Q_UNUSED(len);
}

void DataParser::cmd2_id2_get_data(const QByteArray& buf, quint16 len)
{
    Q_UNUSED(buf);
    Q_UNUSED(len);
}

void DataParser::cmd2_id3_get_data(const QByteArray& buf, quint16 len)
{
    Q_UNUSED(buf);
    Q_UNUSED(len);
}

void DataParser::cmd3_id2_get_data(const QByteArray& buf, quint16 len)
{
    Q_UNUSED(buf);
    Q_UNUSED(len);
}

void DataParser::cmd3_id3_get_data(const QByteArray& buf, quint16 len)
{
    Q_UNUSED(buf);
    Q_UNUSED(len);
}

const DataParser::user_comm_callbak_t DataParser::user_comm_callbak[] = {
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id1), cmd0_id1_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id2), cmd0_id2_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id3), cmd0_id3_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id4), cmd0_id4_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id5), cmd0_id5_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id6), cmd0_id6_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id7), cmd0_id7_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id8), cmd0_id8_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id9), cmd0_id9_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id10), cmd0_id10_get_data },
    { static_cast<quint8>(Cmd::Cmd1), static_cast<quint8>(Id::Id1), cmd1_id1_get_data },
    { static_cast<quint8>(Cmd::Cmd1), static_cast<quint8>(Id::Id2), cmd1_id2_get_data },
    { static_cast<quint8>(Cmd::Cmd1), static_cast<quint8>(Id::Id3), cmd1_id3_get_data },
    { static_cast<quint8>(Cmd::Cmd2), static_cast<quint8>(Id::Id1), cmd2_id1_get_data },
    { static_cast<quint8>(Cmd::Cmd2), static_cast<quint8>(Id::Id2), cmd2_id2_get_data },
    { static_cast<quint8>(Cmd::Cmd2), static_cast<quint8>(Id::Id3), cmd2_id3_get_data },
    { static_cast<quint8>(Cmd::Cmd3), static_cast<quint8>(Id::Id2), cmd3_id2_get_data },
    { static_cast<quint8>(Cmd::Cmd3), static_cast<quint8>(Id::Id3), cmd3_id3_get_data },
};

const int DataParser::user_comm_callbak_count = sizeof(user_comm_callbak) / sizeof(user_comm_callbak[0]);
