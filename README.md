# CommTool - 串口调试工具

一款**跨平台、可扩展、规范化**的通讯调试工具，基于 Qt 6.7+ 开发。通讯底层模型抽离为独立静态库 `CommModelLib`，支持串口数据收发、协议解析、实时图表绘制。

---

## 一、项目结构

```
CommTool/
├── CommModelLib/              # 独立通讯模型工具库（静态库，可单独编译复用）
│   ├── Common/                # 库内部通用枚举、基类、工具
│   │   ├── EnumComm.h         # 通讯枚举、连接状态、串口参数枚举
│   │   ├── Singleton.h        # 单例模板
│   │   └── BaseCommService.cpp/h # 通讯服务抽象基类
│   ├── SerialPort/            # 串口服务实现
│   │   ├── SerialService.cpp
│   │   └── SerialService.h
│   └── docs/                  # 库对外接口说明文档
│       └── SerialService_Interface.md
│
├── Help/                      # 数据解析与通讯实现模块
│   ├── CommImpl.cpp/h         # 状态机帧解析实现
│   ├── DataParser.cpp/h       # 协议解析与数据分发
│   └── crc.cpp/h              # CRC16 校验工具
│
├── View/                      # 视图层（QML界面）
│   ├── Component/             # 自定义组件
│   │   ├── Button.qml         # 统一按钮组件
│   │   └── LineChartWidget.qml # 实时曲线图表组件
│   ├── CommonToolView/        # 通用工具视图
│   │   └── SerialSettingView.qml # 串口参数设置面板
│   ├── Theme/                 # 主题与样式
│   │   ├── ColorTheme.qml     # 颜色主题定义
│   │   └── Toast.qml          # 消息提示组件
│   ├── MainView.qml           # 主界面视图
│   ├── SideBarView.qml        # 侧边栏菜单视图
│   ├── DataShowView.qml       # 数据收发显示视图
│   ├── DataPlotView.qml       # 实时数据绘图视图
│   ├── SettingView.qml        # 设置界面视图
│   ├── HelpView.qml           # 帮助对话框
│   └── AppSettingsDialog.qml  # 应用设置对话框
│
├── ViewModel/                 # 视图模型层（MVVM）
│   ├── CommViewModel.cpp/h    # 串口通讯视图模型，依赖 CommModelLib
│   ├── DataPlotViewModel.cpp/h # 图表数据处理视图模型
│   └── MainViewModel.cpp/h    # 主界面视图模型
│
├── CMakeLists.txt             # 顶层 CMake 构建脚本
├── main.cpp                   # 应用入口
├── qml.qrc                    # QML 资源文件
├── .gitignore                 # Git 忽略配置
└── README.md                  # 项目说明文档
```

**模块职责说明：**

| 层级 | 职责 | 技术实现 |
|------|------|----------|
| **View** | 界面渲染、用户交互、串口参数配置、数据展示 | QML + Qt Quick Controls |
| **ViewModel** | 数据转换、UI与底层库协调、参数透传、状态转发 | C++ QObject，依赖 CommModelLib 对外接口 |
| **CommModelLib** | 底层串口核心业务逻辑、串口硬件操作、数据收发、通讯抽象基类 | C++ 静态库，可脱离主程序单独复用 |
| **Help** | 协议帧解析、CRC 校验、数据回调分发 | C++ 状态机解析 + 回调函数词典 |

---

## 二、CommModelLib 接口说明

### 2.1 全局枚举

**串口波特率**（`EnumComm.h`）：
```cpp
enum class SerialBaudRate
{
    BR_1200, BR_2400, BR_4800, BR_9600, BR_19200,
    BR_38400, BR_57600, BR_115200, BR_230400,
    BR_460800, BR_921600
};
```

**串口参数**（`EnumComm.h`）：
```cpp
enum class SerialDataBits { D5, D6, D7, D8 };
enum class SerialParity { None, Odd, Even, Mark, Space };
enum class SerialStopBits { S1, S1_5, S2 };
enum class SerialFlowControl { None, RTS_CTS, XON_XOFF };
```

**连接状态**（`EnumComm.h`）：
```cpp
enum class CommConnectState
{
    Disconnected, // 未连接
    Connecting,   // 连接中
    Connected,    // 已连接
    Error         // 异常
};
```

### 2.2 基类接口

`BaseCommService` 定义统一通讯接口（`BaseCommService.h`）：
```cpp
virtual CommConnectState getConnectState() const = 0;
virtual void closePort() = 0;
virtual bool sendData(const QByteArray &data) = 0;
```

### 2.3 SerialService 接口

**端口操作**：
```cpp
QStringList getAvailablePortList() const;
void setPortName(const QString &port);
void setBaudRate(SerialBaudRate baud);
void setSerialParams(SerialDataBits, SerialParity, SerialStopBits, SerialFlowControl);
QVariantMap getSerialParams() const;
bool openPort();
```

**信号**：
```cpp
void sigConnectStateChanged(CommConnectState state);
void sigRecvData(const QByteArray &recvBytes);
void sigErrorOccurred(const QString &errMsg);
```

### 2.4 调用示例

```cpp
SerialService serial;

connect(&serial, &SerialService::sigRecvData,
        this, &CommViewModel::onRecvData);
connect(&serial, &SerialService::sigConnectStateChanged,
        this, &CommViewModel::onStateChanged);

serial.setPortName("COM3");
serial.setBaudRate(SerialBaudRate::BR_115200);
serial.openPort();

serial.sendData(QByteArray("test\r\n"));
```

---

## 三、通讯数据流程解析

### 3.1 数据流程总览

```
串口硬件 → SerialService → CommViewModel → DataParser → CommImpl → 数据展示
```

### 3.2 各层详细流程

#### （1）SerialService 层
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 信号 | `sigRecvData(const QByteArray&)` | 底层接收到串口数据后发出 | `CommModelLib/SerialPort/SerialService.h` |

#### （2）CommViewModel 层
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 槽函数 | `onRecvData(const QByteArray&)` | 接收信号并转发解析 | `ViewModel/CommViewModel.cpp` |
| 调用 | `m_dataParser->parse(data)` | 将数据传给 DataParser | `ViewModel/CommViewModel.cpp` |

#### （3）DataParser 层
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 函数 | `parse(const QByteArray&)` | 解析入口 | `Help/DataParser.cpp` |
| 函数 | `user_unpack_data_fun(buf, len)` | 按 cmd/id 匹配回调 | `Help/DataParser.cpp` |
| 信号 | `newAccelData(AccelerationData)` | 加速度数据 | `Help/DataParser.h` |
| 信号 | `newGyroData(GyroscopeData)` | 角速度数据 | `Help/DataParser.h` |
| 信号 | `newSensorData(SensorData)` | 完整传感器数据 | `Help/DataParser.h` |
| 信号 | `newSensorDataPacket(SensorDataPacket)` | 原始数据包 | `Help/DataParser.h` |
| 信号 | `parseError(QString)` | 解析错误 | `Help/DataParser.h` |

#### （4）CommImpl 层（状态机帧解析）
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 函数 | `parse(const QByteArray&)` | 状态机逐字节解析 | `Help/CommImpl.cpp` |
| 信号 | `frameParsed(QByteArray)` | 完整帧解析完成 | `Help/CommImpl.h` |
| 信号 | `parseError(QString)` | 解析失败 | `Help/CommImpl.h` |

#### （5）协议帧结构
```
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────────┬─────────┐
│  SOF    │  SOF1   │   CMD   │    ID   │ v_len_L │ v_len_H │   Data      │ CRC16   │
│  0xAA   │  0x55   │  1字节  │  1字节  │  1字节  │  1字节  │  N字节      │  2字节  │
└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────────┴─────────┘
```

#### （6）回调函数词典

| 回调函数 | cmd | id | 解析数据类型 | 文件位置 |
|----------|-----|----|--------------|----------|
| `cmd0_id1_get_data` | 0x00 | 0x01 | SensorDataPacket | `Help/DataParser.cpp` |
| `cmd0_id2_get_data` | 0x00 | 0x02 | StatusPacket | `Help/DataParser.cpp` |
| `cmd0_id3_get_data` | 0x00 | 0x03 | VisonPacket | `Help/DataParser.cpp` |
| `cmd0_id4_get_data` | 0x00 | 0x04 | HzPacket | `Help/DataParser.cpp` |
| `cmd0_id5_get_data` | 0x00 | 0x05 | SnPacket | `Help/DataParser.cpp` |
| `cmd0_id6_get_data` | 0x00 | 0x06 | AlgorithmPacket | `Help/DataParser.cpp` |
| `cmd0_id7_get_data` | 0x00 | 0x07 | APacket | `Help/DataParser.cpp` |
| `cmd0_id8_get_data` | 0x00 | 0x08 | APacket | `Help/DataParser.cpp` |
| `cmd0_id9_get_data` | 0x00 | 0x09 | CalibrationData | `Help/DataParser.cpp` |
| `cmd0_id10_get_data` | 0x00 | 0x0A | SensorData | `Help/DataParser.cpp` |
| `cmd1_id1_get_data` | 0x01 | 0x01 | APacket | `Help/DataParser.cpp` |
| `cmd1_id2_get_data` | 0x01 | 0x02 | AllDataPacket | `Help/DataParser.cpp` |
| `cmd1_id3_get_data` | 0x01 | 0x03 | PowerPacket | `Help/DataParser.cpp` |

### 3.3 完整数据流转时序图

```
[串口硬件] → SerialService::sigRecvData(rawData)
                ↓
[CommViewModel] → onRecvData(rawData)
                ↓
                m_dataParser->parse(rawData)
                ↓
[DataParser] → parse(rawData) → m_commImpl->parse(rawData)
                ↓
[CommImpl] → 状态机逐字节解析 → frameParsed(frame) [信号]
                ↓
[DataParser] → onFrameParsed(frame) → user_unpack_data_fun()
                ↓ (匹配到 cmd/id)
                newAccelData() / newGyroData() / newSensorDataPacket() [信号]
                ↓
[CommViewModel] → onNewAccelData() / onNewSensorDataPacket()
                ↓
                m_dataPlotViewModel->addDataPoint(timestamp, ax, ay, az, gx, gy, gz, roll, pitch, yaw)
                ↓
[DataPlotViewModel] → 更新图表数据
                ↓
[QML视图] → LineChartWidget 刷新显示曲线（加速度、角速度、姿态角）
```

---

## 四、编译与运行

### 4.1 环境要求

- Qt 6.7+（需包含 Qt SerialPort、Qt Charts 模块）
- CMake 3.24+
- C++17 编译器（MSVC 2022 / GCC 9+ / Clang 10+）

### 4.2 编译步骤

```bash
# 创建构建目录
mkdir build_msvc && cd build_msvc

# 配置 CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# 编译
cmake --build . --config Release

# 运行
./Release/CommTool.exe
```

---

## 五、功能特性

- [x] 串口端口枚举与自动刷新
- [x] 串口参数配置（波特率、数据位、校验位、停止位、流控）
- [x] 数据收发与实时显示
- [x] 十六进制/ASCII 显示切换
- [x] 实时数据图表（加速度、角速度、姿态角）
- [x] 图表暂停/清除/导出功能
- [x] 图例点击隐藏/显示
- [x] 多页面切换（数据显示、数据绘图、设置、帮助）
- [x] 页面状态控制（切换页面时停止渲染，避免卡顿）
- [x] 协议帧解析（支持多种数据类型）
- [x] CRC16 数据完整性校验

---

## 六、技术栈

| 模块 | 技术 |
|------|------|
| 框架 | Qt 6.7+ |
| 界面 | QML + Qt Quick Controls |
| 图表 | Qt Charts |
| 串口 | Qt SerialPort |
| 构建 | CMake |
| 语言 | C++17 + QML |

---

## 七、扩展预留

- 蓝牙通讯服务
- WiFi 通讯服务
- MQTT 协议支持
- HTTP 接口支持

新增通讯服务仅需继承 `BaseCommService`，实现统一接口，上层调用逻辑无需改动。