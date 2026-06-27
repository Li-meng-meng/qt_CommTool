#ifndef COMMMANAGER_H
#define COMMMANAGER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include <QStringList>
#include <QQmlEngine>
#include "CommHandler.h"

class CommManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QStringList availableTypes READ getAvailableTypes NOTIFY availableTypesChanged)
    Q_PROPERTY(QString currentType READ getCurrentType NOTIFY currentTypeChanged)
    Q_PROPERTY(int connectState READ getConnectState NOTIFY connectStateChanged)
    Q_PROPERTY(QStringList deviceList READ getDeviceList NOTIFY deviceListChanged)
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY scanStateChanged)

public:
    explicit CommManager(QObject *parent = nullptr);
    ~CommManager() override;

    QStringList getAvailableTypes() const;
    QString getCurrentType() const;
    int getConnectState() const;
    QStringList getDeviceList() const;
    bool isScanning() const;

    void registerHandler(CommHandler *handler);
    CommHandler *getHandler(const QString &type) const;

public slots:
    void setCurrentType(const QString &type);
    bool openPort(const QVariantMap &params);
    void closePort();
    bool sendData(const QByteArray &data);
    void refreshDevices();
    void startScan();
    void stopScan();

signals:
    void availableTypesChanged(const QStringList &types);
    void currentTypeChanged(const QString &type);
    void connectStateChanged(int state);
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &message);
    void deviceListChanged(const QStringList &devices);
    void scanStateChanged(bool scanning);

private slots:
    void onHandlerStateChanged(CommConnectState state);
    void onHandlerDataReceived(const QByteArray &data);
    void onHandlerErrorOccurred(const QString &message);

private:
    void clearHandlers();

private:
    QMap<QString, CommHandler *> m_handlers;
    CommHandler *m_currentHandler;
    QString m_currentType;
    QStringList m_deviceList;
    bool m_isScanning;
};

#endif // COMMMANAGER_H
