#include "BluetoothService.h"
#include <QDebug>
#include <QBluetoothAddress>

BluetoothService::BluetoothService(QObject *parent)
    : BaseCommService(parent),
      m_discoveryAgent(nullptr),
      m_controller(nullptr),
      m_service(nullptr),
      m_connectState(CommConnectState::Disconnected),
      m_scanning(false)
{
}

BluetoothService::~BluetoothService()
{
    cleanup();
}

CommConnectState BluetoothService::getConnectState() const
{
    return m_connectState;
}

void BluetoothService::closePort()
{
    if (m_controller) {
        m_controller->disconnectFromDevice();
    }
    cleanup();
}

bool BluetoothService::sendData(const QByteArray &data)
{
    if (m_connectState != CommConnectState::Connected) {
        emit sigErrorOccurred("未连接蓝牙设备");
        return false;
    }

    if (!m_writeChar.isValid()) {
        emit sigErrorOccurred("写特征未找到");
        return false;
    }

    m_service->writeCharacteristic(m_writeChar, data, QLowEnergyService::WriteWithoutResponse);
    return true;
}

void BluetoothService::startScan()
{
    if (m_scanning)
        return;

    if (m_discoveryAgent) {
        delete m_discoveryAgent;
    }

    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BluetoothService::onDeviceDiscovered);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
            this, &BluetoothService::onScanError);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &BluetoothService::onScanFinished);

    m_scanning = true;
    emit sigScanStateChanged(true);
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BluetoothService::stopScan()
{
    if (m_discoveryAgent && m_scanning) {
        m_discoveryAgent->stop();
    }
}

bool BluetoothService::isScanning() const
{
    return m_scanning;
}

bool BluetoothService::connectToDevice(const QString &deviceAddress)
{
    if (m_connectState == CommConnectState::Connected) {
        closePort();
    }

    QBluetoothAddress addr(deviceAddress);
    if (addr.toString().isEmpty()) {
        emit sigErrorOccurred("无效的蓝牙地址");
        return false;
    }

    QBluetoothDeviceInfo deviceInfo(addr, "", 0);

    m_controller = QLowEnergyController::createCentral(deviceInfo, this);

    connect(m_controller, &QLowEnergyController::connected,
            this, &BluetoothService::onConnected);
    connect(m_controller, &QLowEnergyController::disconnected,
            this, &BluetoothService::onDisconnected);
    connect(m_controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::errorOccurred),
            this, &BluetoothService::onControllerError);
    connect(m_controller, &QLowEnergyController::serviceDiscovered,
            this, &BluetoothService::onServiceDiscovered);
    connect(m_controller, &QLowEnergyController::discoveryFinished,
            m_controller, [this]() {
        if (!m_service && m_connectState == CommConnectState::Connecting) {
            emit sigErrorOccurred("未找到指定服务");
            closePort();
        }
    });

    m_connectState = CommConnectState::Connecting;
    emit sigConnectStateChanged(m_connectState);

    m_controller->connectToDevice();
    return true;
}

QString BluetoothService::getConnectedDeviceName() const
{
    return m_deviceName;
}

QString BluetoothService::getConnectedDeviceAddress() const
{
    return m_deviceAddress;
}

void BluetoothService::setServiceUuid(const QBluetoothUuid &uuid)
{
    m_serviceUuid = uuid;
}

void BluetoothService::setWriteCharUuid(const QBluetoothUuid &uuid)
{
    m_writeCharUuid = uuid;
}

void BluetoothService::setNotifyCharUuid(const QBluetoothUuid &uuid)
{
    m_notifyCharUuid = uuid;
}

void BluetoothService::onDeviceDiscovered(const QBluetoothDeviceInfo &device)
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        QString name = device.name();
        QString address = device.address().toString();
        emit sigDeviceFound(name, address);
    }
}

void BluetoothService::onScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    QString errorMsg = QString("扫描错误: %1").arg((int)error);
    emit sigErrorOccurred(errorMsg);
    m_scanning = false;
    emit sigScanStateChanged(false);
}

void BluetoothService::onScanFinished()
{
    m_scanning = false;
    emit sigScanStateChanged(false);
}

void BluetoothService::onConnected()
{
    m_connectState = CommConnectState::Connected;
    emit sigConnectStateChanged(m_connectState);
    m_controller->discoverServices();
}

void BluetoothService::onDisconnected()
{
    m_connectState = CommConnectState::Disconnected;
    emit sigConnectStateChanged(m_connectState);
    cleanup();
}

void BluetoothService::onControllerError(QLowEnergyController::Error error)
{
    QString errorMsg = QString("连接错误: %1").arg((int)error);
    emit sigErrorOccurred(errorMsg);
    closePort();
}

void BluetoothService::onServiceDiscovered(const QBluetoothUuid &newService)
{
    if (!m_serviceUuid.isNull() && newService == m_serviceUuid) {
        m_service = m_controller->createServiceObject(newService, this);
        if (m_service) {
            connect(m_service, &QLowEnergyService::stateChanged,
                    this, &BluetoothService::onServiceStateChanged);
            connect(m_service, &QLowEnergyService::characteristicChanged,
                    this, &BluetoothService::onCharacteristicChanged);
            connect(m_service, &QLowEnergyService::characteristicWritten,
                    this, &BluetoothService::onCharacteristicWrite);
            m_service->discoverDetails();
        }
    }
}

void BluetoothService::onServiceStateChanged(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::ServiceDiscovered) {
        QList<QLowEnergyCharacteristic> chars = m_service->characteristics();

        for (const QLowEnergyCharacteristic &ch : chars) {
            if (!m_writeCharUuid.isNull() && ch.uuid() == m_writeCharUuid) {
                m_writeChar = ch;
            }
            if (!m_notifyCharUuid.isNull() && ch.uuid() == m_notifyCharUuid) {
                m_notifyChar = ch;
                QLowEnergyDescriptor desc = ch.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
                if (desc.isValid()) {
                    m_service->writeDescriptor(desc, QByteArray::fromHex("0100"));
                }
            }
        }

        if (!m_writeChar.isValid()) {
            emit sigErrorOccurred("写特征未找到");
        }
        if (!m_notifyChar.isValid()) {
            emit sigErrorOccurred("通知特征未找到");
        }
    }
}

void BluetoothService::onCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    if (info.uuid() == m_notifyCharUuid) {
        emit sigRecvData(value);
    }
}

void BluetoothService::onCharacteristicWrite(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    Q_UNUSED(info);
    Q_UNUSED(value);
}

void BluetoothService::cleanup()
{
    if (m_notifyChar.isValid()) {
        QLowEnergyDescriptor desc = m_notifyChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
        if (desc.isValid() && m_service) {
            m_service->writeDescriptor(desc, QByteArray::fromHex("0000"));
        }
        m_notifyChar = QLowEnergyCharacteristic();
    }

    if (m_writeChar.isValid()) {
        m_writeChar = QLowEnergyCharacteristic();
    }

    if (m_service) {
        delete m_service;
        m_service = nullptr;
    }

    if (m_controller) {
        delete m_controller;
        m_controller = nullptr;
    }

    if (m_discoveryAgent) {
        delete m_discoveryAgent;
        m_discoveryAgent = nullptr;
    }

    m_scanning = false;
    emit sigScanStateChanged(false);

    if (m_connectState != CommConnectState::Disconnected) {
        m_connectState = CommConnectState::Disconnected;
        emit sigConnectStateChanged(m_connectState);
    }

    m_deviceName.clear();
    m_deviceAddress.clear();
}
