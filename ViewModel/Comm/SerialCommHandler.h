#ifndef SERIALCOMMHANDLER_H
#define SERIALCOMMHANDLER_H

#include "CommHandler.h"
#include "../../CommModelLib/SerialPort/SerialService.h"
#include "../../CommModelLib/Common/EnumComm.h"

class SerialCommHandler : public CommHandler
{
    Q_OBJECT

public:
    explicit SerialCommHandler(QObject *parent = nullptr);
    ~SerialCommHandler() override;

    QString type() const override;
    CommConnectState getConnectState() const override;
    bool sendData(const QByteArray &data) override;
    void closePort() override;
    bool openPort(const QVariantMap &params) override;
    QVariantMap getCurrentParams() const override;
    QStringList getAvailableDevices() const override;

private slots:
    void onStateChanged(CommConnectState state);
    void onRecvData(const QByteArray &data);
    void onErrorOccurred(const QString &message);

private:
    SerialBaudRate baudRateFromInt(int baud) const;
    SerialDataBits dataBitsFromInt(int bits) const;
    SerialParity parityFromInt(int index) const;
    SerialStopBits stopBitsFromInt(int index) const;
    SerialFlowControl flowControlFromInt(int index) const;

private:
    SerialService m_serial;
    QString m_portName;
    int m_baudRate;
    int m_dataBits;
    int m_parity;
    int m_stopBits;
    int m_flowControl;
};

#endif // SERIALCOMMHANDLER_H
