#ifndef CRC_H
#define CRC_H

#include <QByteArray>

class CRC {
public:
    static quint8 CRC8(const QByteArray& data);
    static quint8 CRC8(const quint8* buf, quint16 len);

    static quint16 CRC16(const QByteArray& data);
    static quint16 CRC16(const quint8* buf, quint16 len);

    static bool VerifyCRC8(const QByteArray& data);
    static bool VerifyCRC16(const QByteArray& data);

private:
    CRC() = delete;
    ~CRC() = delete;

    static constexpr quint8 crc8Init = 0x77;
    static constexpr quint16 crc16Init = 0x3692;
    static constexpr quint16 crc16Poly = 0x8408;

    static const quint8 crc8Table[];
};

#endif // CRC_H
