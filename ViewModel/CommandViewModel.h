#ifndef COMMANDVIEWMODEL_H
#define COMMANDVIEWMODEL_H

#include <QObject>
#include <QVariantList>
#include <QQmlEngine>
#include <QTimer>
#include "../Help/DataParser.h"

class CommandViewModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QVariantList commandGroups READ commandGroups CONSTANT)
    Q_PROPERTY(QVariantMap selectedCommand READ selectedCommand WRITE setSelectedCommand NOTIFY selectedCommandChanged)
    Q_PROPERTY(QString generatedHex READ generatedHex NOTIFY generatedHexChanged)
    Q_PROPERTY(QString receivedHex READ receivedHex NOTIFY receivedHexChanged)
    Q_PROPERTY(QString receivedDataText READ receivedDataText NOTIFY receivedDataTextChanged)
    Q_PROPERTY(int selectedOp READ selectedOp WRITE setSelectedOp NOTIFY selectedOpChanged)
    Q_PROPERTY(int selectedData1 READ selectedData1 WRITE setSelectedData1 NOTIFY selectedData1Changed)
    Q_PROPERTY(QVariantList chartConfigs READ chartConfigs NOTIFY chartConfigsChanged)
    Q_PROPERTY(bool chartPaused READ chartPaused WRITE setChartPaused NOTIFY chartPausedChanged)
    Q_PROPERTY(int chartDataVersion READ chartDataVersion NOTIFY chartDataVersionChanged)

public:
    explicit CommandViewModel(QObject *parent = nullptr);
    ~CommandViewModel() override;

    QVariantList commandGroups() const;
    QVariantMap selectedCommand() const;
    QString generatedHex() const;
    QString receivedHex() const;
    QString receivedDataText() const;
    int selectedOp() const;
    int selectedData1() const;
    QVariantList chartConfigs() const;

    Q_INVOKABLE void generateHex();
    Q_INVOKABLE void copyHex();
    Q_INVOKABLE void clearReceivedHex();
    Q_INVOKABLE QVariantList getChartTimeValues(int chartIndex) const;
    Q_INVOKABLE QVariantList getChartSeriesValues(int chartIndex, int seriesIndex) const;
    Q_INVOKABLE void clearChartData();
    Q_INVOKABLE void setChartPaused(bool paused);
    Q_INVOKABLE bool chartPaused() const;
    Q_INVOKABLE int chartDataVersion() const;

public slots:
    void sendCommand(quint8 cmd, quint8 id);
    void setSelectedCommand(const QVariantMap& cmd);
    void setSelectedOp(int op);
    void setSelectedData1(int data1);
    void appendReceivedData(const QByteArray& data);
    void handleReceivedData(quint8 cmd, quint8 id, const QByteArray& data);

signals:
    void commandSent(quint8 cmd, quint8 id);
    void sendDataRequested(const QByteArray& data);
    void selectedCommandChanged();
    void generatedHexChanged();
    void receivedHexChanged();
    void receivedDataTextChanged();
    void selectedOpChanged();
    void selectedData1Changed();
    void hexCopied();
    void chartConfigsChanged();
    void chartDataChanged();
    void chartDataVersionChanged();
    void chartPausedChanged(bool paused);

private:
    void initCommandGroups();
    QByteArray buildPayload(quint8 cmd, quint8 id);
    void flushReceiveBuffer();
    void updateChartConfigs();
    void addChartValue(int chartIndex, int seriesIndex, float value, float timeOffset = 0);

    QVariantList m_commandGroups;
    QVariantMap m_selectedCommand;
    QString m_generatedHex;
    QString m_receivedHex;
    QString m_receivedDataText;
    QString m_pendingDataText;
    QString m_pendingHex;
    bool m_receiveBufferDirty;
    bool m_dataTextDirty;
    bool m_hexdDirty;
    int m_selectedOp = 0;
    int m_selectedData1 = 0;
    bool m_chartPaused = false;
    qint64 m_chartStartTime = 0;
    int m_maxChartPoints = 5000;
    int m_chartDataVersion = 0;
    QVariantList m_chartConfigs;
    QList<QVariantList> m_chartTimeLists;
    QList<QList<QVariantList>> m_chartSeriesLists;

    QTimer* m_flushTimer;
};

#endif // COMMANDVIEWMODEL_H
