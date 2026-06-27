#include "CommManager.h"
#include "BluetoothCommHandler.h"

CommManager::CommManager(QObject *parent)
    : QObject(parent)
    , m_currentHandler(nullptr)
    , m_isScanning(false)
{
    qDebug() << "[CommManager] constructor created";
}

CommManager::~CommManager()
{
    clearHandlers();
}

QStringList CommManager::getAvailableTypes() const
{
    return m_handlers.keys();
}

QString CommManager::getCurrentType() const
{
    return m_currentType;
}

int CommManager::getConnectState() const
{
    if (m_currentHandler) {
        return static_cast<int>(m_currentHandler->getConnectState());
    }
    return static_cast<int>(CommConnectState::Disconnected);
}

QStringList CommManager::getDeviceList() const
{
    return m_deviceList;
}

bool CommManager::isScanning() const
{
    return m_isScanning;
}

void CommManager::registerHandler(CommHandler *handler)
{
    if (!handler) {
        qDebug() << "[CommManager] registerHandler: handler is null";
        return;
    }
    QString type = handler->type();
    qDebug() << "[CommManager] registerHandler: type=" << type;
    if (!m_handlers.contains(type)) {
        m_handlers[type] = handler;
        handler->setParent(this);
        bool conn1 = connect(handler, &CommHandler::connectStateChanged,
            this, &CommManager::onHandlerStateChanged);
        bool conn2 = connect(handler, &CommHandler::dataReceived,
            this, &CommManager::onHandlerDataReceived);
        bool conn3 = connect(handler, &CommHandler::errorOccurred,
            this, &CommManager::onHandlerErrorOccurred);
        qDebug() << "[CommManager] registerHandler: connections - state:" << conn1
                 << ", data:" << conn2 << ", error:" << conn3;

        // 蓝牙特殊信号
        if (type == "bluetooth") {
            BluetoothCommHandler *btHandler = static_cast<BluetoothCommHandler*>(handler);
            connect(btHandler, &BluetoothCommHandler::deviceListChanged,
                this, [this]() {
                    if (m_currentType == "bluetooth") {
                        m_deviceList = m_currentHandler->getAvailableDevices();
                        emit deviceListChanged(m_deviceList);
                    }
                });
            connect(btHandler, &BluetoothCommHandler::scanStateChanged,
                this, [this](bool scanning) {
                    m_isScanning = scanning;
                    emit scanStateChanged(scanning);
                });
            qDebug() << "[CommManager] registerHandler: bluetooth signals connected";
        }

        emit availableTypesChanged(getAvailableTypes());
        qDebug() << "[CommManager] registerHandler: handler registered successfully";
    } else {
        qDebug() << "[CommManager] registerHandler: handler type already exists";
    }
}

CommHandler *CommManager::getHandler(const QString &type) const
{
    return m_handlers.value(type, nullptr);
}

void CommManager::setCurrentType(const QString &type)
{
    if (m_currentType == type) {
        return;
    }

    if (m_currentHandler) {
        m_currentHandler->closePort();
        disconnect(m_currentHandler, &CommHandler::connectStateChanged,
            this, &CommManager::onHandlerStateChanged);
        disconnect(m_currentHandler, &CommHandler::dataReceived,
            this, &CommManager::onHandlerDataReceived);
        disconnect(m_currentHandler, &CommHandler::errorOccurred,
            this, &CommManager::onHandlerErrorOccurred);
    }

    m_currentType = type;
    m_currentHandler = m_handlers.value(type, nullptr);
    qDebug() << "[CommManager] setCurrentType: handler set, type:" << type 
             << ", handler:" << (m_currentHandler ? "not null" : "null");

    refreshDevices();
    emit currentTypeChanged(m_currentType);
    emit connectStateChanged(getConnectState());
}

bool CommManager::openPort(const QVariantMap &params)
{
    if (!m_currentHandler) {
        return false;
    }
    return m_currentHandler->openPort(params);
}

void CommManager::closePort()
{
    if (m_currentHandler) {
        m_currentHandler->closePort();
    }
}

bool CommManager::sendData(const QByteArray &data)
{
    if (!m_currentHandler) {
        return false;
    }
    return m_currentHandler->sendData(data);
}

void CommManager::refreshDevices()
{
    qDebug() << "[CommManager] refreshDevices called, currentType:" << m_currentType;
    if (m_currentHandler) {
        m_deviceList = m_currentHandler->getAvailableDevices();
        qDebug() << "[CommManager] deviceList updated, count:" << m_deviceList.size();
        emit deviceListChanged(m_deviceList);
    } else {
        m_deviceList.clear();
        qDebug() << "[CommManager] no current handler, deviceList cleared";
        emit deviceListChanged(m_deviceList);
    }
}

void CommManager::onHandlerStateChanged(CommConnectState state)
{
    emit connectStateChanged(static_cast<int>(state));
}

void CommManager::onHandlerDataReceived(const QByteArray &data)
{
    qDebug() << "[CommManager] onHandlerDataReceived, size:" << data.size() 
             << ", hex:" << data.toHex().left(32);
    emit dataReceived(data);
}

void CommManager::onHandlerErrorOccurred(const QString &message)
{
    emit errorOccurred(message);
}

void CommManager::clearHandlers()
{
    qDeleteAll(m_handlers.values());
    m_handlers.clear();
    m_currentHandler = nullptr;
    m_currentType.clear();
    m_isScanning = false;
}

void CommManager::startScan()
{
    qDebug() << "[CommManager] startScan called, currentType:" << m_currentType;
    if (m_currentType == "bluetooth") {
        BluetoothCommHandler *btHandler = static_cast<BluetoothCommHandler*>(m_currentHandler);
        if (btHandler) {
            btHandler->startScan();
        }
    }
}

void CommManager::stopScan()
{
    qDebug() << "[CommManager] stopScan called, currentType:" << m_currentType;
    if (m_currentType == "bluetooth") {
        BluetoothCommHandler *btHandler = static_cast<BluetoothCommHandler*>(m_currentHandler);
        if (btHandler) {
            btHandler->stopScan();
        }
    }
}
