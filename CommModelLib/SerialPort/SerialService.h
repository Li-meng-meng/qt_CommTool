#ifndef SERIALSERVICE_H
#define SERIALSERVICE_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QVariantMap>
#include "../Common/BaseCommService.h"
#include "../Common/EnumComm.h"

/**
 * @brief 串口通讯服务类
 * 
 * 继承 BaseCommService，实现串口通讯的具体逻辑。
 * 封装 Qt6::SerialPort，提供端口枚举、参数配置、数据收发等功能。
 * 
 * 收到的数据会自动入队到 BaseCommService 的线程安全队列中，
 * 上层可通过信号或轮询方式获取数据。
 */
class COMMMODELLIB_EXPORT SerialService : public BaseCommService
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit SerialService(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~SerialService() override;

    /**
     * @brief 获取当前连接状态
     * @return 连接状态枚举值
     */
    CommConnectState getConnectState() const override;

    /**
     * @brief 关闭串口
     */
    void closePort() override;

    /**
     * @brief 发送字节数据
     * @param data 要发送的字节数据
     * @return true=发送成功, false=发送失败（端口未打开）
     */
    bool sendData(const QByteArray &data) override;

    /**
     * @brief 获取本机全部可用串口名称列表
     * @return 串口名称列表（如 COM1, COM2...）
     */
    QStringList getAvailablePortList() const;

    /**
     * @brief 设置串口名
     * @param port 串口名称（如 "COM3"）
     */
    void setPortName(const QString &port);

    /**
     * @brief 设置波特率
     * @param baud 波特率枚举值
     */
    void setBaudRate(SerialBaudRate baud);

    /**
     * @brief 设置串口参数
     * @param db 数据位
     * @param pr 校验位
     * @param sb 停止位
     * @param fc 流控
     */
    void setSerialParams(SerialDataBits db, SerialParity pr, SerialStopBits sb, SerialFlowControl fc);

    /**
     * @brief 获取当前完整串口参数
     * @return 参数映射表，包含 portName、baudRate、dataBits、parity、stopBits、flowControl
     */
    QVariantMap getSerialParams() const;

    /**
     * @brief 打开串口
     * @return true=打开成功, false=打开失败（端口不存在、被占用等）
     */
    bool openPort();

signals:
    /**
     * @brief 端口列表更新信号（预留）
     * @param ports 更新后的端口列表
     */
    void sigPortListUpdated(const QStringList &ports);

private slots:
    /**
     * @brief 串口数据接收槽函数
     * 
     * 读取串口缓冲区数据，自动入队并发送信号。
     */
    void onReadyRead();

    /**
     * @brief 串口错误发生槽函数
     * @param error 错误类型
     */
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serialPort;          ///< Qt 串口对象
    QString m_portName;                ///< 串口名称
    SerialBaudRate m_baudRate;         ///< 波特率
    SerialDataBits m_dataBits;         ///< 数据位
    SerialParity m_parity;             ///< 校验位
    SerialStopBits m_stopBits;         ///< 停止位
    SerialFlowControl m_flowControl;   ///< 流控
    CommConnectState m_connectState;   ///< 当前连接状态
};

#endif // SERIALSERVICE_H