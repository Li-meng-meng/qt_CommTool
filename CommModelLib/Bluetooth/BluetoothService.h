#ifndef BLUETOOTHSERVICE_H
#define BLUETOOTHSERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QBluetoothUuid>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>
#include "../Common/BaseCommService.h"
#include "../Common/EnumComm.h"

class COMMMODELLIB_EXPORT BluetoothService : public BaseCommService
{
    Q_OBJECT
public:
    explicit BluetoothService(QObject *parent = nullptr);
    ~BluetoothService() override;

    CommConnectState getConnectState() const override;
    void closePort() override;
    bool sendData(const QByteArray &data) override;

    void startScan();
    void stopScan();
    bool isScanning() const;

    bool connectToDevice(const QString &deviceAddress);
    QString getConnectedDeviceName() const;
    QString getConnectedDeviceAddress() const;

    void setServiceUuid(const QBluetoothUuid &uuid);
    void setWriteCharUuid(const QBluetoothUuid &uuid);
    void setNotifyCharUuid(const QBluetoothUuid &uuid);

signals:
    void sigScanStateChanged(bool scanning);
    void sigDeviceFound(const QString &name, const QString &address);

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &device);
    void onScanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void onScanFinished();

    void onConnected();
    void onDisconnected();
    void onControllerError(QLowEnergyController::Error error);

    void onServiceDiscovered(const QBluetoothUuid &newService);
    void onServiceStateChanged(QLowEnergyService::ServiceState newState);
    void onCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value);
    void onCharacteristicWrite(const QLowEnergyCharacteristic &info, const QByteArray &value);

private:
    void cleanup();

private:
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QLowEnergyController *m_controller;
    QLowEnergyService *m_service;

    QLowEnergyCharacteristic m_writeChar;
    QLowEnergyCharacteristic m_notifyChar;

    QBluetoothUuid m_serviceUuid;
    QBluetoothUuid m_writeCharUuid;
    QBluetoothUuid m_notifyCharUuid;

    CommConnectState m_connectState;
    bool m_scanning;
    QString m_deviceName;
    QString m_deviceAddress;
};

#endif // BLUETOOTHSERVICE_H
