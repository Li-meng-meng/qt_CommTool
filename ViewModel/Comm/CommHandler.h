#ifndef COMMHANDLER_H
#define COMMHANDLER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include "../../CommModelLib/Common/EnumComm.h"

class CommHandler : public QObject
{
    Q_OBJECT

public:
    explicit CommHandler(QObject *parent = nullptr) : QObject(parent) {}
    ~CommHandler() override = default;

    virtual QString type() const = 0;
    virtual CommConnectState getConnectState() const = 0;
    virtual bool sendData(const QByteArray &data) = 0;
    virtual void closePort() = 0;
    virtual bool openPort(const QVariantMap &params) = 0;
    virtual QVariantMap getCurrentParams() const = 0;
    virtual QStringList getAvailableDevices() const = 0;

signals:
    void connectStateChanged(CommConnectState state);
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &message);
};

#endif // COMMHANDLER_H
