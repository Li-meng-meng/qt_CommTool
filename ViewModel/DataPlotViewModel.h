#ifndef DATAPLOTVIEWMODEL_H
#define DATAPLOTVIEWMODEL_H

#include <QObject>
#include <QVariantList>
#include <QMutex>
#include <QQmlEngine>
#include <QTimer>
#include "../Help/DataParser.h"

class DataPlotViewModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QVariantList timeValues READ timeValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList xValues READ xValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList yValues READ yValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList zValues READ zValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList gxValues READ gxValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList gyValues READ gyValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList gzValues READ gzValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList rollValues READ rollValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList pitchValues READ pitchValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList yawValues READ yawValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList mxValues READ mxValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList myValues READ myValues NOTIFY dataChanged)
    Q_PROPERTY(QVariantList mzValues READ mzValues NOTIFY dataChanged)
    Q_PROPERTY(bool isPaused READ isPaused NOTIFY isPausedChanged)
    Q_PROPERTY(bool isViewActive READ isViewActive WRITE setViewActive NOTIFY isViewActiveChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorOccurred)

public:
    explicit DataPlotViewModel(QObject *parent = nullptr);
    ~DataPlotViewModel() override;

    QVariantList timeValues() const;
    QVariantList xValues() const;
    QVariantList yValues() const;
    QVariantList zValues() const;
    QVariantList gxValues() const;
    QVariantList gyValues() const;
    QVariantList gzValues() const;
    QVariantList rollValues() const;
    QVariantList pitchValues() const;
    QVariantList yawValues() const;
    QVariantList mxValues() const;
    QVariantList myValues() const;
    QVariantList mzValues() const;

    bool isPaused() const;
    bool isViewActive() const;
    QString errorMessage() const;

    void setViewActive(bool active);

public slots:
    void addDataPoint(qint64 timestamp, float ax, float ay, float az,
                      float gx, float gy, float gz,
                      float roll, float pitch, float yaw,
                      float mx, float my, float mz);
    void clearChart();
    void setPaused(bool paused);
    bool exportChartData(const QString& filePath);
    void onNewAccelerationData(const AccelerationData& data);
    void onBatchDataParsed(const QList<AccelerationData>& data);
    void onParseError(const QString& error);

signals:
    void dataChanged();
    void isPausedChanged(bool paused);
    void isViewActiveChanged(bool active);
    void errorOccurred(const QString& message);

private slots:
    void onUpdateTimeout();

private:
    void trimDataLists();

    QVariantList m_timeValues;
    QVariantList m_xValues;
    QVariantList m_yValues;
    QVariantList m_zValues;
    QVariantList m_gxValues;
    QVariantList m_gyValues;
    QVariantList m_gzValues;
    QVariantList m_rollValues;
    QVariantList m_pitchValues;
    QVariantList m_yawValues;
    QVariantList m_mxValues;
    QVariantList m_myValues;
    QVariantList m_mzValues;

    bool m_isPaused;
    bool m_isViewActive;
    QString m_errorMessage;
    qint64 m_startTimestamp;
    int m_maxPoints;
    int m_pendingDataCount;

    mutable QMutex m_dataMutex;
    QTimer* m_updateTimer;
};

#endif // DATAPLOTVIEWMODEL_H