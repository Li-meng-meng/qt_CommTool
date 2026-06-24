# CommModelLib SerialService 对外接口文档

## 1. 概述

本库为独立静态库，第一版仅实现串口通讯服务，所有硬件操作、数据收发、端口枚举逻辑封装内部；上层仅依赖头文件 `SerialService.h`，不感知底层平台串口适配细节。

依赖模块：Qt6::SerialPort

扩展规范：所有通讯服务继承 `BaseCommService`，统一接口约束。

## 2. 全局枚举（EnumComm.h）

### 2.1 串口波特率

```cpp
enum class SerialBaudRate
{
    BR_9600,
    BR_19200,
    BR_38400,
    BR_57600,
    BR_115200
};
```

### 2.2 数据位、校验位、停止位、流控

```cpp
enum class SerialDataBits { D8 };
enum class SerialParity { None, Odd, Even };
enum class SerialStopBits { S1, S2 };
enum class SerialFlowControl { None };
```

### 2.3 通讯连接状态

```cpp
enum class CommConnectState
{
    Disconnected, // 未打开端口
    Connecting,   // 打开中
    Connected,    // 正常连接
    Error         // 端口异常/断开
};
```

## 3. 基类 BaseCommService 统一对外虚接口

### 3.1 通讯控制接口

```cpp
// 获取当前连接状态
virtual CommConnectState getConnectState() const = 0;
// 关闭通讯端口
virtual void closePort() = 0;
// 发送字节数据
virtual bool sendData(const QByteArray &data) = 0;
```

### 3.2 线程安全数据队列接口

```cpp
// 检查队列是否有待处理数据
virtual bool hasPendingData() const;
// 获取并移除队列头部数据（线程安全）
virtual QByteArray getPendingData();
// 获取队列中数据条数
virtual int pendingDataSize() const;
// 清空队列
virtual void clearPendingData();
```

## 4. SerialService 独有对外接口

### 4.1 端口枚举

```cpp
// 获取本机全部可用串口名称列表
QStringList getAvailablePortList() const;
```

### 4.2 串口参数设置

```cpp
// 设置串口名
void setPortName(const QString &port);
// 设置波特率
void setBaudRate(SerialBaudRate baud);
// 设置数据位/校验/停止位/流控
void setSerialParams(SerialDataBits db, SerialParity pr, SerialStopBits sb, SerialFlowControl fc);
// 获取当前完整串口参数
QVariantMap getSerialParams() const;
```

### 4.3 端口打开

```cpp
// 打开串口，返回true=打开成功
bool openPort();
```

## 5. 对外信号（上层ViewModel绑定使用）

```cpp
// 连接状态变更
void sigConnectStateChanged(CommConnectState state);
// 底层收到串口数据
void sigRecvData(const QByteArray &recvBytes);
// 底层异常信息
void sigErrorOccurred(const QString &errMsg);
// 端口枚举完成回调
void sigPortListUpdated(const QStringList &ports);
```

## 6. 调用示例（ViewModel层标准用法）

```cpp
// 1. 实例化串口服务
SerialService m_serial;

// 2. 绑定底层信号到ViewModel槽函数
connect(&m_serial, &SerialService::sigConnectStateChanged,
        this, &CommViewModel::onStateChange);
connect(&m_serial, &SerialService::sigRecvData,
        this, &CommViewModel::onRecvData);

// 3. 配置串口参数
m_serial.setPortName("COM3");
m_serial.setBaudRate(SerialBaudRate::BR_115200);

// 4. 打开串口
bool ret = m_serial.openPort();

// 5. 发送数据
m_serial.sendData(QByteArray("test\r\n"));

// 6. 关闭端口
m_serial.closePort();
```

## 7. 异常说明

1. 端口不存在、被占用：`openPort()` 返回 false，触发 `sigErrorOccurred`；
2. 未打开端口调用 sendData：直接返回 false，抛出错误信号；
3. 平台差异：Windows返回COMx，Android返回USB串口设备名，上层无需区分。

## 8. 扩展预留

新增蓝牙/WiFi服务仅需继承 `BaseCommService`，实现统一虚接口，上层ViewModel调用逻辑无改动。