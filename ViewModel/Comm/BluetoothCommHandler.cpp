#include "BluetoothCommHandler.h"
#include <QDebug>

BluetoothCommHandler::BluetoothCommHandler(QObject *parent)
    : CommHandler(parent)
    , m_bluetooth(new BluetoothService(this))
    , m_serviceUuid("0000ffe0-0000-1000-8000-00805f9b34fb")
    , m_writeCharUuid("0000ffe1-0000-1000-8000-00805f9b34fb")
    , m_notifyCharUuid("0000ffe1-0000-1000-8000-00805f9b34fb")
{
    connect(m_bluetooth, &BluetoothService::sigConnectStateChanged,
        this, &BluetoothCommHandler::onStateChanged);
    connect(m_bluetooth, &BluetoothService::sigRecvData,
        this, &BluetoothCommHandler::onRecvData);
    connect(m_bluetooth, &BluetoothService::sigErrorOccurred,
        this, &BluetoothCommHandler::onErrorOccurred);
    connect(m_bluetooth, &BluetoothService::sigDeviceFound,
        this, [this](const QString &name, const QString &address) {
            QString deviceEntry = name + " (" + address + ")";
            if (!m_deviceList.contains(deviceEntry)) {
                m_deviceList.append(deviceEntry);
                qDebug() << "[BluetoothCommHandler] Device found:" << deviceEntry;
                emit deviceListChanged();
            }
        });
    connect(m_bluetooth, &BluetoothService::sigScanStateChanged,
        this, [this](bool scanning) {
            qDebug() << "[BluetoothCommHandler] Scan state changed:" << scanning;
            emit scanStateChanged(scanning);
        });
}

BluetoothCommHandler::~BluetoothCommHandler()
{
    if (m_bluetooth) {
        m_bluetooth->closePort();
    }
}

QString BluetoothCommHandler::type() const
{
    return "bluetooth";
}

CommConnectState BluetoothCommHandler::getConnectState() const
{
    return m_bluetooth->getConnectState();
}

bool BluetoothCommHandler::sendData(const QByteArray &data)
{
    return m_bluetooth->sendData(data);
}

void BluetoothCommHandler::closePort()
{
    m_bluetooth->closePort();
}

bool BluetoothCommHandler::openPort(const QVariantMap &params)
{
    m_deviceAddress = params.value("deviceAddress").toString();
    m_serviceUuid = params.value("serviceUuid", m_serviceUuid).toString();
    m_writeCharUuid = params.value("writeCharUuid", m_writeCharUuid).toString();
    m_notifyCharUuid = params.value("notifyCharUuid", m_notifyCharUuid).toString();

    m_bluetooth->setServiceUuid(QBluetoothUuid(m_serviceUuid));
    m_bluetooth->setWriteCharUuid(QBluetoothUuid(m_writeCharUuid));
    m_bluetooth->setNotifyCharUuid(QBluetoothUuid(m_notifyCharUuid));

    return m_bluetooth->connectToDevice(m_deviceAddress);
}

QVariantMap BluetoothCommHandler::getCurrentParams() const
{
    QVariantMap params;
    params["deviceAddress"] = m_deviceAddress;
    params["serviceUuid"] = m_serviceUuid;
    params["writeCharUuid"] = m_writeCharUuid;
    params["notifyCharUuid"] = m_notifyCharUuid;
    return params;
}

QStringList BluetoothCommHandler::getAvailableDevices() const
{
    return m_deviceList;
}

void BluetoothCommHandler::startScan()
{
    qDebug() << "[BluetoothCommHandler] startScan called";
    m_deviceList.clear();
    emit deviceListChanged();
    m_bluetooth->startScan();
}

void BluetoothCommHandler::stopScan()
{
    qDebug() << "[BluetoothCommHandler] stopScan called";
    m_bluetooth->stopScan();
}

bool BluetoothCommHandler::isScanning() const
{
    return m_bluetooth->isScanning();
}

void BluetoothCommHandler::onStateChanged(CommConnectState state)
{
    emit connectStateChanged(state);
}

void BluetoothCommHandler::onRecvData(const QByteArray &data)
{
    emit dataReceived(data);
}

void BluetoothCommHandler::onErrorOccurred(const QString &message)
{
    emit errorOccurred(message);
}
