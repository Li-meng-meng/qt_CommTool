#ifndef BASECOMMSERVICE_H
#define BASECOMMSERVICE_H

#include <QObject>
#include <QByteArray>
#include "EnumComm.h"

#ifdef COMMMODELLIB_STATIC
#  define COMMMODELLIB_EXPORT
#else
#  define COMMMODELLIB_EXPORT __declspec(dllexport)
#endif

class COMMMODELLIB_EXPORT BaseCommService : public QObject
{
    Q_OBJECT
public:
    explicit BaseCommService(QObject *parent = nullptr);
    virtual ~BaseCommService() override;

    virtual CommConnectState getConnectState() const = 0;
    virtual void closePort() = 0;
    virtual bool sendData(const QByteArray &data) = 0;

signals:
    void sigConnectStateChanged(CommConnectState state);
    void sigErrorOccurred(const QString &error);
    void sigRecvData(const QByteArray &data);
};

#endif // BASECOMMSERVICE_H