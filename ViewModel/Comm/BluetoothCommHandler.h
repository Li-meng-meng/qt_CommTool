#ifndef BLUETOOTHCOMMHANDLER_H
#define BLUETOOTHCOMMHANDLER_H

#include "CommHandler.h"
#include "../../CommModelLib/Bluetooth/BluetoothService.h"
#include "../../CommModelLib/Common/EnumComm.h"

class BluetoothCommHandler : public CommHandler
{
    Q_OBJECT

public:
    explicit BluetoothCommHandler(QObject *parent = nullptr);
    ~BluetoothCommHandler() override;

    QString type() const override;
    CommConnectState getConnectState() const override;
    bool sendData(const QByteArray &data) override;
    void closePort() override;
    bool openPort(const QVariantMap &params) override;
    QVariantMap getCurrentParams() const override;
    QStringList getAvailableDevices() const override;

    void startScan();
    void stopScan();
    bool isScanning() const;

signals:
    void deviceListChanged();
    void scanStateChanged(bool scanning);

private slots:
    void onStateChanged(CommConnectState state);
    void onRecvData(const QByteArray &data);
    void onErrorOccurred(const QString &message);

private:
    BluetoothService *m_bluetooth;
    QString m_deviceAddress;
    QString m_serviceUuid;
    QString m_writeCharUuid;
    QString m_notifyCharUuid;
    QStringList m_deviceList;
};

#endif // BLUETOOTHCOMMHANDLER_H
