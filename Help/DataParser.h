#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QString>
#include "CommImpl.h"

#pragma pack(push, 1)
struct user_head_t {
    quint8 sof;
    quint8 sof1;
    quint8 cmd;
    quint8 id;
    quint16 v_len;
};

struct send_user_head_t {
    quint8 sof;
    quint8 sof1;
    quint8 cmd;
    quint8 id;
    quint8 op;
    quint16 v_len;
};

struct SensorDataPacket {
    user_head_t head;
    quint32 PushFrequency;
    quint16 Year;
    quint8 Month;
    quint8 Day;
    quint8 Hour;
    quint8 Minute;
    quint8 Second;
    float Acceleration[4];
    float AngularVelocity[4];
    float Temperature;
    float EulerAngles[3];
    float Quaternion[4];
    float Magnetometer[3];
    quint16 Crc16;
};

struct StatusPacket {
    user_head_t head;
    quint8 UserA;
    quint8 BatteryLevel;
    quint16 Crc16;
};

struct AllDataPacket {
    user_head_t head;
    quint32 PushFrequency;
    float Acceleration[4];
    float AngularVelocity[4];
    float Temperature;
    float EulerAngles[3];
    float Quaternion[4];
    float Magnetometer[3];
    quint8 BatteryLevel;
    quint8 UserA;
    quint16 Crc16;
};

struct VisonPacket {
    user_head_t head;
    quint32 vison;
    quint16 Crc16;
};

struct HzPacket {
    user_head_t head;
    quint8 hz;
    quint16 Crc16;
};

struct SnPacket {
    user_head_t head;
    quint8 data;
    quint16 Crc16;
};

struct AlgorithmPacket {
    user_head_t head;
    quint8 algorithm;
    quint16 Crc16;
};

struct APacket {
    user_head_t head;
    quint8 a;
    quint16 Crc16;
};

struct PowerPacket {
    user_head_t head;
    quint8 power;
    quint16 Crc16;
};

struct CalibrationData {
    user_head_t head;
    float Progress;
    quint8 IsValid;
    quint8 DirCount;
    quint16 SampleCount;
    quint16 Crc16;
};

struct SensorData {
    user_head_t head;
    quint64 Timestamp;
    float Acc[3];
    float Gyro[3];
    float Angle[3];
    quint16 Crc16;
};

#pragma pack(pop)

struct AccelerationData {
    qint64 timestamp;
    float ax;
    float ay;
    float az;
};

struct GyroscopeData {
    qint64 timestamp;
    float gx;
    float gy;
    float gz;
};

struct AngleData {
    qint64 timestamp;
    float roll;
    float pitch;
    float yaw;
};

class DataParser : public QObject
{
    Q_OBJECT

public:
    explicit DataParser(QObject *parent = nullptr);
    ~DataParser() override;

    void parse(const QByteArray& rawData);
    void clear();

signals:
    void newAccelData(const AccelerationData& data);
    void newGyroData(const GyroscopeData& data);
    void newAngleData(const AngleData& data);
    void newSensorData(const ::SensorData& data);
    void newSensorDataPacket(const SensorDataPacket& data);
    void newStatusData(const StatusPacket& data);
    void newAllData(const AllDataPacket& data);
    void parseError(const QString& error);

private slots:
    void onFrameParsed(const QByteArray& frame);

private:
    enum class Cmd : quint8 {
        Cmd0 = 0x00,
        Cmd1 = 0x01,
        Cmd2 = 0x02,
        Cmd3 = 0x03,
        Cmd4 = 0x04,
        Cmd5 = 0x05,
        Cmd6 = 0x06,
        Cmd7 = 0x07,
        Cmd8 = 0x08,
        Cmd9 = 0x09,
        Cmd10 = 0x10
    };

    enum class Id : quint8 {
        Id0 = 0x00,
        Id1 = 0x01,
        Id2 = 0x02,
        Id3 = 0x03,
        Id4 = 0x04,
        Id5 = 0x05,
        Id6 = 0x06,
        Id7 = 0x07,
        Id8 = 0x08,
        Id9 = 0x09,
        Id10 = 0x10
    };

    using userFunPtr = void(*)(const QByteArray&, quint16);

    struct user_comm_callbak_t {
        quint8 cmd;
        quint8 id;
        userFunPtr fun;
    };

    void user_unpack_data_fun(const QByteArray& buf, quint16 len);

    static void cmd0_id1_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id2_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id3_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id4_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id5_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id6_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id7_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id8_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id9_get_data(const QByteArray& buf, quint16 len);
    static void cmd0_id10_get_data(const QByteArray& buf, quint16 len);
    static void cmd1_id1_get_data(const QByteArray& buf, quint16 len);
    static void cmd1_id2_get_data(const QByteArray& buf, quint16 len);
    static void cmd1_id3_get_data(const QByteArray& buf, quint16 len);
    static void cmd2_id1_get_data(const QByteArray& buf, quint16 len);
    static void cmd2_id2_get_data(const QByteArray& buf, quint16 len);
    static void cmd2_id3_get_data(const QByteArray& buf, quint16 len);
    static void cmd3_id2_get_data(const QByteArray& buf, quint16 len);
    static void cmd3_id3_get_data(const QByteArray& buf, quint16 len);

    static const user_comm_callbak_t user_comm_callbak[];
    static const int user_comm_callbak_count;

    CommImpl* m_commImpl;

    static DataParser* s_instance;
};

#endif // DATAPARSER_H
