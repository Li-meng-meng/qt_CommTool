#ifndef COMMVIEWMODEL_H
#define COMMVIEWMODEL_H

#include <QObject>
#include <QByteArray>
#include <QStringList>
#include <QQmlEngine>
#include "../CommModelLib/SerialPort/SerialService.h"
#include "../CommModelLib/Common/EnumComm.h"
#include "DataParser.h"
#include "DataPlotViewModel.h"

class CommViewModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString portName READ getPortName WRITE setPortName NOTIFY portNameChanged)
    Q_PROPERTY(int baudRate READ getBaudRate WRITE setBaudRate NOTIFY baudRateChanged)
    Q_PROPERTY(int dataBits READ getDataBits WRITE setDataBits NOTIFY dataBitsChanged)
    Q_PROPERTY(int parity READ getParity WRITE setParity NOTIFY parityChanged)
    Q_PROPERTY(int stopBits READ getStopBits WRITE setStopBits NOTIFY stopBitsChanged)
    Q_PROPERTY(int flowControl READ getFlowControl WRITE setFlowControl NOTIFY flowControlChanged)
    Q_PROPERTY(int connectState READ getConnectState NOTIFY connectStateChanged)
    Q_PROPERTY(QString receiveData READ getReceiveData NOTIFY receiveDataChanged)
    Q_PROPERTY(QString sendHistory READ getSendHistory NOTIFY sendHistoryChanged)
    Q_PROPERTY(QStringList portList READ getPortList NOTIFY portListChanged)
    Q_PROPERTY(bool hexDisplay READ getHexDisplay WRITE setHexDisplay NOTIFY hexDisplayChanged)
    Q_PROPERTY(bool hexSend READ getHexSend WRITE setHexSend NOTIFY hexSendChanged)
    Q_PROPERTY(bool parseEnabled READ getParseEnabled WRITE setParseEnabled NOTIFY parseEnabledChanged)
    Q_PROPERTY(bool isPlotViewActive READ getIsPlotViewActive WRITE setIsPlotViewActive NOTIFY isPlotViewActiveChanged)
    Q_PROPERTY(bool isShowViewActive READ getIsShowViewActive WRITE setIsShowViewActive NOTIFY isShowViewActiveChanged)
    Q_PROPERTY(QObject* dataPlotViewModel READ getDataPlotViewModel CONSTANT)

public:
    explicit CommViewModel(QObject *parent = nullptr);
    ~CommViewModel() override;

    QString getPortName() const;
    void setPortName(const QString& name);

    int getBaudRate() const;
    void setBaudRate(int baudRate);

    int getDataBits() const;
    void setDataBits(int dataBits);

    int getParity() const;
    void setParity(int parity);

    int getStopBits() const;
    void setStopBits(int stopBits);

    int getFlowControl() const;
    void setFlowControl(int flowControl);

    int getConnectState() const;
    QString getReceiveData() const;
    QString getSendHistory() const;
    QStringList getPortList() const;
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

public slots:
    void openSerialPort(const QString& portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl);
    void closeSerialPort();
    void sendData(const QString& data);
    void refreshPortList();
    void clearReceiveData();
    void clearSendHistory();

signals:
    void portNameChanged(const QString& name);
    void baudRateChanged(int baudRate);
    void dataBitsChanged(int dataBits);
    void parityChanged(int parity);
    void stopBitsChanged(int stopBits);
    void flowControlChanged(int flowControl);
    void connectStateChanged(int state);
    void receiveDataChanged(const QString& data);
    void sendHistoryChanged(const QString& history);
    void portListChanged(const QStringList& ports);
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
    void onStateChanged(CommConnectState state);
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
    SerialBaudRate baudRateFromInt(int baud) const;
    SerialDataBits dataBitsFromInt(int bits) const;
    SerialParity parityFromInt(int index) const;
    SerialStopBits stopBitsFromInt(int index) const;
    SerialFlowControl flowControlFromInt(int index) const;

    SerialService m_serial;
    QString m_portName;
    int m_baudRate;
    int m_dataBits;
    int m_parity;
    int m_stopBits;
    int m_flowControl;
    int m_connectState;
    QString m_receiveData;
    QString m_sendHistory;
    QStringList m_portList;
    bool m_hexDisplay;
    bool m_hexSend;
    bool m_parseEnabled;
    bool m_isPlotViewActive;
    bool m_isShowViewActive;

    DataParser* m_dataParser;
    DataPlotViewModel* m_dataPlotViewModel;
};

#endif // COMMVIEWMODEL_H
