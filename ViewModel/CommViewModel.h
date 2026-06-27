#ifndef COMMVIEWMODEL_H
#define COMMVIEWMODEL_H

#include <QObject>
#include <QByteArray>
#include <QStringList>
#include <QQmlEngine>
#include "../CommModelLib/Common/EnumComm.h"
#include "Comm/CommManager.h"
#include "Comm/SerialCommHandler.h"
#include "DataParser.h"
#include "DataPlotViewModel.h"
#include "CommandViewModel.h"

class CommViewModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int connectState READ getConnectState NOTIFY connectStateChanged)
    Q_PROPERTY(QString receiveData READ getReceiveData NOTIFY receiveDataChanged)
    Q_PROPERTY(QString sendHistory READ getSendHistory NOTIFY sendHistoryChanged)
    Q_PROPERTY(bool hexDisplay READ getHexDisplay WRITE setHexDisplay NOTIFY hexDisplayChanged)
    Q_PROPERTY(bool hexSend READ getHexSend WRITE setHexSend NOTIFY hexSendChanged)
    Q_PROPERTY(bool parseEnabled READ getParseEnabled WRITE setParseEnabled NOTIFY parseEnabledChanged)
    Q_PROPERTY(bool isPlotViewActive READ getIsPlotViewActive WRITE setIsPlotViewActive NOTIFY isPlotViewActiveChanged)
    Q_PROPERTY(bool isShowViewActive READ getIsShowViewActive WRITE setIsShowViewActive NOTIFY isShowViewActiveChanged)
    Q_PROPERTY(QObject* dataPlotViewModel READ getDataPlotViewModel CONSTANT)
    Q_PROPERTY(QObject* commandViewModel READ getCommandViewModel CONSTANT)
    Q_PROPERTY(QObject* commManager READ getCommManager CONSTANT)

public:
    explicit CommViewModel(QObject *parent = nullptr);
    ~CommViewModel() override;

    int getConnectState() const;
    QString getReceiveData() const;
    QString getSendHistory() const;
    bool getHexDisplay() const;
    void setHexDisplay(bool hex);
    bool getHexSend() const;
    void setHexSend(bool hex);
    bool getParseEnabled() const;
    void setParseEnabled(bool enabled);
    bool getIsPlotViewActive() const;
    void setIsPlotViewActive(bool active);
    bool getIsShowViewActive() const;
    void setIsShowViewActive(bool active);

    DataPlotViewModel* getDataPlotViewModel() const;
    CommandViewModel* getCommandViewModel() const;
    CommManager* getCommManager() const;

public slots:
    void sendData(const QString& data);
    void clearReceiveData();
    void clearSendHistory();

signals:
    void connectStateChanged(int state);
    void receiveDataChanged(const QString& data);
    void sendHistoryChanged(const QString& history);
    void hexDisplayChanged(bool hex);
    void hexSendChanged(bool hex);
    void parseEnabledChanged(bool enabled);
    void isPlotViewActiveChanged(bool active);
    void isShowViewActiveChanged(bool active);
    void errorOccurred(const QString& message);
    void accelerationDataReady(const AccelerationData& data);
    void gyroscopeDataReady(const GyroscopeData& data);
    void sensorDataReady(const ::SensorData& data);

private slots:
    void onStateChanged(int state);
    void onRecvData(const QByteArray& data);
    void onErrorOccurred(const QString& message);
    void onNewAccelData(const AccelerationData& data);
    void onNewGyroData(const GyroscopeData& data);
    void onNewSensorData(const SensorData& data);
    void onNewSensorDataPacket(const SensorDataPacket& data);
    void onNewAllData(const AllDataPacket& data);
    void onParseError(const QString& error);

private:
    QString byteArrayToHexString(const QByteArray& data) const;
    QByteArray hexStringToByteArray(const QString& hex) const;
    void flushReceiveBuffer();

private:
    CommManager* m_commManager;
    int m_connectState;
    QString m_receiveData;
    QString m_receiveBuffer;
    QString m_sendHistory;
    bool m_hexDisplay;
    bool m_hexSend;
    bool m_parseEnabled;
    bool m_isPlotViewActive;
    bool m_isShowViewActive;
    bool m_receiveBufferDirty;

    QTimer* m_receiveFlushTimer;

    DataParser* m_dataParser;
    DataPlotViewModel* m_dataPlotViewModel;
    CommandViewModel* m_commandViewModel;
};

#endif // COMMVIEWMODEL_H
