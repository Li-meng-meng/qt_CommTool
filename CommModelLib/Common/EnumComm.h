#ifndef ENUMCOMM_H
#define ENUMCOMM_H

#include <QMetaType>

enum class CommConnectState
{
    Disconnected,
    Connecting,
    Connected,
    Error
};

enum class SerialBaudRate
{
    BR_1200,
    BR_2400,
    BR_4800,
    BR_9600,
    BR_19200,
    BR_38400,
    BR_57600,
    BR_115200,
    BR_230400,
    BR_460800,
    BR_921600
};

enum class SerialDataBits { D5, D6, D7, D8 };
enum class SerialParity { None, Odd, Even, Mark, Space };
enum class SerialStopBits { S1, S1_5, S2 };
enum class SerialFlowControl { None, RTS_CTS, XON_XOFF };

Q_DECLARE_METATYPE(CommConnectState)
Q_DECLARE_METATYPE(SerialBaudRate)
Q_DECLARE_METATYPE(SerialDataBits)
Q_DECLARE_METATYPE(SerialParity)
Q_DECLARE_METATYPE(SerialStopBits)
Q_DECLARE_METATYPE(SerialFlowControl)

#endif // ENUMCOMM_H