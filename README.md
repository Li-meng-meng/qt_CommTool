# CommTool - 串口调试工具

一款**跨平台、可扩展、规范化**的通讯调试工具，基于 Qt 6.7+ 开发。通讯底层模型抽离为独立静态库 `CommModelLib`，支持串口数据收发、协议解析、实时图表绘制、命令面板管理。

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
│   ├── CommImpl.cpp/h         # 状态机帧解析实现（独立线程）
│   ├── DataParser.cpp/h       # 协议解析与数据分发
│   └── crc.cpp/h              # CRC16 校验工具
│
├── View/                      # 视图层（QML界面）
│   ├── Component/             # 自定义组件
│   │   ├── Button.qml         # 统一按钮组件（primary/normal/danger/success）
│   │   ├── LineChartWidget.qml # 实时曲线图表组件（支持折叠）
│   │   └── ChartStatusOverlay.qml # 图表状态覆盖层
│   ├── CommonToolView/        # 通用工具视图
│   │   └── SerialSettingView.qml # 串口参数设置面板
│   ├── Theme/                 # 主题与样式
│   │   ├── ColorTheme.qml     # 颜色主题定义（亮/暗双主题）
│   │   ├── Toast.qml          # 消息提示组件
│   │   └── qmldir             # QML单例声明
│   ├── MainView.qml           # 主界面视图（页面切换容器）
│   ├── SideBarView.qml        # 侧边栏菜单视图
│   ├── DataShowView.qml       # 数据收发显示视图
│   ├── DataPlotView.qml       # 实时数据绘图视图
│   ├── CommandMenuView.qml    # 命令菜单视图（指令发送/HEX生成/数据解析）
│   ├── SettingView.qml        # 设置界面视图
│   ├── HelpView.qml           # 帮助对话框
│   └── AppSettingsDialog.qml  # 应用设置对话框
│
├── ViewModel/                 # 视图模型层（MVVM）
│   ├── CommViewModel.cpp/h    # 串口通讯视图模型，核心调度中心
│   ├── CommandViewModel.cpp/h # 命令面板视图模型（指令生成/图表数据）
│   ├── DataPlotViewModel.cpp/h # 图表数据处理视图模型（默认数据）
│   ├── MainViewModel.cpp/h    # 主界面视图模型
│   └── AppSettingsViewModel.cpp/h # 应用设置视图模型
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
| **View** | 界面渲染、用户交互、串口参数配置、数据展示、命令面板 | QML + Qt Quick Controls |
| **ViewModel** | 数据转换、UI与底层库协调、参数透传、状态转发、图表数据管理 | C++ QObject，依赖 CommModelLib 对外接口 |
| **CommModelLib** | 底层串口核心业务逻辑、串口硬件操作、数据收发、通讯抽象基类 | C++ 静态库，可脱离主程序单独复用 |
| **Help** | 协议帧解析、CRC 校验、数据回调分发、独立解析线程 | C++ 状态机解析 + 回调函数词典 + QThread |

---

## 二、文件详细说明

### 2.1 CommModelLib 层

#### EnumComm.h
通讯相关枚举定义：
- `SerialBaudRate`：串口波特率（1200 ~ 921600）
- `SerialDataBits`：数据位（5/6/7/8）
- `SerialParity`：校验位（None/Odd/Even/Mark/Space）
- `SerialStopBits`：停止位（1/1.5/2）
- `SerialFlowControl`：流控（None/RTS_CTS/XON_XOFF）
- `CommConnectState`：连接状态（Disconnected/Connecting/Connected/Error）

#### BaseCommService.h
通讯服务抽象基类，定义统一接口：
```cpp
virtual CommConnectState getConnectState() const = 0;
virtual void closePort() = 0;
virtual bool sendData(const QByteArray &data) = 0;
```

#### SerialService.h / SerialService.cpp
串口服务实现，继承 `BaseCommService`。

**主要方法：**
| 方法 | 说明 |
|------|------|
| `getAvailablePortList()` | 获取可用串口列表 |
| `setPortName()` | 设置串口名 |
| `setBaudRate()` | 设置波特率 |
| `setSerialParams()` | 设置串口参数（数据位/校验/停止位/流控） |
| `openPort()` | 打开串口 |
| `closePort()` | 关闭串口 |
| `sendData()` | 发送数据 |

**主要信号：**
| 信号 | 说明 |
|------|------|
| `sigConnectStateChanged(CommConnectState)` | 连接状态变化 |
| `sigRecvData(const QByteArray&)` | 接收到数据 |
| `sigErrorOccurred(const QString&)` | 发生错误 |

### 2.2 Help 层

#### CommImpl.h / CommImpl.cpp
状态机帧解析器，运行在独立线程中。

**核心机制：**
- 生产者-消费者模式：`m_receiveQueue` 队列 + `QMutex` + `QWaitCondition`
- 逐字节状态机解析：SOF → SOF1 → CMD → ID → v_len → Data → CRC16
- 最大帧长：256 字节

**主要方法：**
| 方法 | 说明 |
|------|------|
| `enqueue(rawData)` | 数据入队（生产者，主线程调用） |
| `clear()` | 清空解析状态和队列 |
| `ByteArrayToStructure<T>()` | 字节数组转结构体模板函数 |

**主要信号：**
| 信号 | 说明 |
|------|------|
| `frameParsed(const QByteArray& frame)` | 完整帧解析完成 |
| `parseError(const QString& error)` | 解析错误 |

#### DataParser.h / DataParser.cpp
协议数据解析与分发器，维护回调函数词典。

**数据结构：**
| 结构体 | 说明 |
|--------|------|
| `user_head_t` | 协议帧头（SOF/SOF1/CMD/ID/v_len） |
| `send_user_head_t` | 发送帧头（含 OP 字段） |
| `SensorDataPacket` | 传感器数据包（cmd0_id1） |
| `AllDataPacket` | 全部数据（cmd1_id2） |
| `StatusPacket` | 状态数据（cmd0_id2） |
| `SensorData` | 传感器推送数据2（cmd0_id10） |
| `AccelerationData` | 加速度数据 |
| `GyroscopeData` | 角速度数据 |
| `AngleData` | 姿态角数据 |

**主要方法：**
| 方法 | 说明 |
|------|------|
| `parse(rawData)` | 解析入口，转发给 CommImpl |
| `clear()` | 清空解析器 |
| `user_unpack_data_fun()` | 按 cmd/id 匹配回调函数 |

**主要信号：**
| 信号 | 说明 |
|------|------|
| `newAccelData(AccelerationData)` | 加速度数据 |
| `newGyroData(GyroscopeData)` | 角速度数据 |
| `newAngleData(AngleData)` | 姿态角数据 |
| `newSensorData(SensorData)` | 传感器数据（cmd0_id10） |
| `newSensorDataPacket(SensorDataPacket)` | 传感器数据包（cmd0_id1） |
| `newStatusData(StatusPacket)` | 状态数据 |
| `newAllData(AllDataPacket)` | 全部数据（cmd1_id2） |
| `newReceivedData(cmd, id, data)` | 通用接收数据（cmd/id透传） |
| `parseError(QString)` | 解析错误 |

**回调函数词典：**

| 回调函数 | cmd | id | 解析数据类型 | 发射信号 |
|----------|-----|----|--------------|----------|
| `cmd0_id1_get_data` | 0x00 | 0x01 | SensorDataPacket | newSensorDataPacket |
| `cmd0_id2_get_data` | 0x00 | 0x02 | StatusPacket | newStatusData |
| `cmd0_id3_get_data` | 0x00 | 0x03 | VisonPacket | - |
| `cmd0_id4_get_data` | 0x00 | 0x04 | HzPacket | - |
| `cmd0_id5_get_data` | 0x00 | 0x05 | SnPacket | - |
| `cmd0_id6_get_data` | 0x00 | 0x06 | AlgorithmPacket | - |
| `cmd0_id7_get_data` | 0x00 | 0x07 | APacket | - |
| `cmd0_id8_get_data` | 0x00 | 0x08 | APacket | - |
| `cmd0_id9_get_data` | 0x00 | 0x09 | CalibrationData | - |
| `cmd0_id10_get_data` | 0x00 | 0x10 | SensorData | newSensorData |
| `cmd1_id1_get_data` | 0x01 | 0x01 | APacket | - |
| `cmd1_id2_get_data` | 0x01 | 0x02 | AllDataPacket | newAllData |
| `cmd1_id3_get_data` | 0x01 | 0x03 | PowerPacket | - |

#### crc.h / crc.cpp
CRC16 校验工具函数。

### 2.3 ViewModel 层

#### CommViewModel.h / CommViewModel.cpp
**核心调度中心**，协调串口、解析器、图表、命令面板。

**QML 属性：**
| 属性 | 类型 | 说明 |
|------|------|------|
| `portName` | QString | 串口名称 |
| `baudRate` | int | 波特率 |
| `connectState` | int | 连接状态 |
| `receiveData` | QString | 接收数据显示文本 |
| `hexDisplay` | bool | 十六进制显示开关 |
| `parseEnabled` | bool | 协议解析开关 |
| `isPlotViewActive` | bool | 绘图页是否激活 |
| `isShowViewActive` | bool | 数据显示页是否激活 |
| `dataPlotViewModel` | QObject* | 图表视图模型（默认模式） |
| `commandViewModel` | QObject* | 命令视图模型 |

**主要槽函数：**
| 槽函数 | 说明 |
|--------|------|
| `openSerialPort()` | 打开串口 |
| `closeSerialPort()` | 关闭串口 |
| `sendData(data)` | 发送数据 |
| `onRecvData(data)` | 接收串口数据 → 转发解析 + 显示 |
| `onStateChanged(state)` | 连接状态变化 |
| `onNewAccelData(data)` | 新加速度数据 → DataPlotViewModel |
| `onNewSensorDataPacket(data)` | 传感器数据包 → CommandViewModel 图表 |
| `onNewAllData(data)` | 全部数据 → CommandViewModel 图表 |
| `refreshPortList()` | 刷新串口列表 |

**主要信号：**
| 信号 | 说明 |
|------|------|
| `connectStateChanged(int)` | 连接状态变化 |
| `receiveDataChanged(QString)` | 接收数据变化 |
| `portListChanged(QStringList)` | 串口列表变化 |
| `errorOccurred(QString)` | 错误发生 |

#### CommandViewModel.h / CommandViewModel.cpp
命令面板视图模型，管理指令生成、接收数据显示、图表数据。

**QML 属性：**
| 属性 | 类型 | 说明 |
|------|------|------|
| `commandGroups` | QVariantList | 命令分组列表 |
| `selectedCommand` | QVariantMap | 当前选中的命令 |
| `generatedHex` | QString | 生成的 HEX 字符串 |
| `receivedHex` | QString | 接收到的 HEX（最新帧） |
| `receivedDataText` | QString | 解析后的结构化数据文本 |
| `chartConfigs` | QVariantList | 图表配置列表 |
| `chartPaused` | bool | 图表暂停状态 |
| `chartDataVersion` | int | 图表数据版本号（触发QML刷新） |

**主要方法：**
| 方法 | 说明 |
|------|------|
| `generateHex()` | 根据选中命令生成 HEX 帧 |
| `sendCommand(cmd, id)` | 发送命令（通过 sendDataRequested 信号） |
| `handleReceivedData(cmd, id, data)` | 处理匹配 cmd/id 的接收数据 |
| `getChartTimeValues(chartIndex)` | 获取图表时间轴数据 |
| `getChartSeriesValues(chartIndex, seriesIndex)` | 获取图曲线数据 |
| `clearChartData()` | 清除图表数据 |

**主要信号：**
| 信号 | 说明 |
|------|------|
| `sendDataRequested(QByteArray)` | 请求发送数据 → CommViewModel |
| `chartDataChanged()` | 图表数据变化 → DataPlotView 刷新 |
| `chartConfigsChanged()` | 图表配置变化 |
| `selectedCommandChanged()` | 选中命令变化 |
| `receivedHexChanged()` | 接收 HEX 变化 |
| `receivedDataTextChanged()` | 接收数据文本变化 |

#### DataPlotViewModel.h / DataPlotViewModel.cpp
默认图表数据视图模型（DataPlotView 默认模式使用）。

**QML 属性：**
| 属性 | 类型 | 说明 |
|------|------|------|
| `timeValues` | QVariantList | 时间轴数据 |
| `xValues/yValues/zValues` | QVariantList | 加速度 X/Y/Z |
| `gxValues/gyValues/gzValues` | QVariantList | 角速度 X/Y/Z |
| `rollValues/pitchValues/yawValues` | QVariantList | 姿态角 Roll/Pitch/Yaw |
| `mxValues/myValues/mzValues` | QVariantList | 磁场 X/Y/Z |
| `isPaused` | bool | 暂停状态 |
| `isViewActive` | bool | 视图是否激活 |

**主要方法：**
| 方法 | 说明 |
|------|------|
| `addDataPoint()` | 添加数据点（加速度+角速度+姿态角+磁场） |
| `clearChart()` | 清除所有图表数据 |
| `setPaused()` | 设置暂停 |

**主要信号：**
| 信号 | 说明 |
|------|------|
| `dataChanged()` | 数据变化 → 图表刷新 |
| `isPausedChanged(bool)` | 暂停状态变化 |

#### MainViewModel.h / MainViewModel.cpp
主界面视图模型，管理当前视图切换。

| 属性 | 类型 | 说明 |
|------|------|------|
| `currentView` | int | 当前视图索引 |

#### AppSettingsViewModel.h / AppSettingsViewModel.cpp
应用设置视图模型（暗黑模式等全局设置）。

### 2.4 View 层

#### MainView.qml
主界面容器，管理页面切换（串口设置、数据显示、数据绘图、命令菜单、设置）。

#### SideBarView.qml
侧边栏导航菜单，使用 Button 组件。

#### DataShowView.qml
数据收发显示视图，支持 ASCII/HEX 切换、发送历史、显示长度限制。

#### DataPlotView.qml
实时数据绘图视图，动态显示多个 LineChartWidget，支持根据选中命令切换图表配置。

#### CommandMenuView.qml
命令菜单视图，支持：
- 命令分组树形展示
- 命令参数配置
- HEX 帧生成与显示
- 接收数据解析展示
- 匹配 cmd/id 的数据过滤

#### LineChartWidget.qml
实时曲线图表组件：
- 单图表3条曲线
- 折叠/展开功能（点击标题左侧箭头）
- 暂停/继续/清除/导出按钮
- 图例点击显隐
- 折叠时停止刷新，展开时重建数据

#### Button.qml
统一按钮组件，支持4种类型：
- `primary`：主按钮（蓝色填充）
- `normal`：普通按钮（边框+透明背景）
- `danger`：危险按钮（红色填充）
- `success`：成功按钮（绿色填充）

每种类型均包含：normal / hover / pressed / disabled 四种状态。

#### ColorTheme.qml
颜色主题单例，亮/暗双主题动态切换。

---

## 三、数据处理信号流程

### 3.1 发送数据流程

```
用户点击按钮
    ↓
CommandViewModel::sendCommand(cmd, id)
    ├── 构造协议帧（SOF+SOF1+CMD+ID+OP+v_len+Data+CRC16）
    └── 发射信号 sendDataRequested(frame)
            ↓
CommViewModel::sendData(data) [槽函数]
            ↓
SerialService::sendData(data)
            ↓
串口硬件发送
```

### 3.2 接收数据流程（主路径）

```
串口硬件接收
    ↓
SerialService::sigRecvData(rawData) [信号]
    ↓
CommViewModel::onRecvData(rawData) [槽函数]
    ├── 追加到显示缓冲区（1000ms定时器刷新UI）
    └── m_dataParser->parse(rawData)
            ↓
DataParser::parse(rawData)
            ↓
CommImpl::enqueue(rawData) [入队]
            ↓
[独立解析线程] CommImpl::onParseLoop()
            ↓
状态机逐字节解析 → CRC校验通过
            ↓
CommImpl::frameParsed(frame) [信号]
    ↓
DataParser::onFrameParsed(frame) [槽函数]
    ↓
user_unpack_data_fun(buf, len) → 按 cmd/id 匹配回调
    ↓
匹配成功 → 发射对应数据信号
```

### 3.3 传感器数据 → 图表流程

```
DataParser::newSensorDataPacket(SensorDataPacket) [信号]
    ├──→ CommViewModel::onNewSensorDataPacket() [槽]
    │       └── commandViewModel->handleReceivedData(cmd, id, data)
    │               └── CommandViewModel 内部：
    │                   ├── 格式化 receivedDataText（结构化显示）
    │                   ├── 更新 receivedHex（最新帧HEX）
    │                   └── 解析数据后调用 addChartValue() → 更新图表数据
    │                           └── chartDataVersion++ → 发射 chartDataChanged()
    │                                   ↓
    │                           DataPlotView Connections 触发
    │                                   ↓
    │                           LineChartWidget::updateChart() 刷新曲线
    │
    └──→ (cmd0_id1 同时更新 DataPlotViewModel 默认模式数据)

DataParser::newAllData(AllDataPacket) [信号]
    └──→ CommViewModel::onNewAllData() [槽]
            └── commandViewModel->handleReceivedData(cmd, id, data)
                    └── 同上，更新 CommandViewModel 图表
```

### 3.4 命令数据接收过滤流程

```
DataParser::newReceivedData(cmd, id, data) [信号]
    ↓
CommViewModel → 转发给 CommandViewModel::handleReceivedData()
    ↓
判断 cmd/id 是否与当前选中命令匹配
    ├── 匹配 → 更新 m_pendingHex / m_pendingDataText
    │           （1000ms定时器刷新到 receivedHex / receivedDataText）
    └── 不匹配 → 忽略（保证只显示对应命令的数据）
```

### 3.5 页面切换与性能控制

```
MainView 页面切换
    ↓
设置 isPlotViewActive / isShowViewActive
    ↓
CommViewModel 中对应标志位更新
    ├── isShowViewActive = false → 停止数据显示刷新
    ├── isPlotViewActive = false → 停止图表定时器刷新
    └── 页面重新激活时 → 恢复刷新
```

### 3.6 完整数据流转时序图

```
[串口硬件]
    ↓
SerialService::sigRecvData(rawData)
    ↓
[CommViewModel] onRecvData(rawData)
    ├─→ 显示缓冲追加（定时器1000ms刷新DataShowView）
    └─→ m_dataParser->parse(rawData)
            ↓
[DataParser] parse(rawData)
            ↓
[CommImpl] enqueue(rawData) → m_receiveQueue [线程安全队列]
            ↓
[独立解析线程] onParseLoop()
    ├─ 状态机逐字节解析（SOF→SOF1→CMD→ID→v_len→Data→CRC）
    └─ frameParsed(frame) [信号]
            ↓
[DataParser] onFrameParsed(frame)
            ↓
user_unpack_data_fun() → 按 cmd/id 查表匹配回调
            ↓
┌─────────────────────────────────────────┐
│ 匹配到不同类型，发射不同信号：             │
│  - newAccelData()                        │
│  - newGyroData()                         │
│  - newSensorDataPacket()                 │
│  - newAllData()                          │
│  - newReceivedData(cmd,id,data)          │
└─────────────────────────────────────────┘
            ↓
[CommViewModel] 对应槽函数接收
    ├─→ DataPlotViewModel::addDataPoint() → 默认模式图表
    └─→ CommandViewModel::handleReceivedData()
            ├─ 格式化 receivedDataText
            ├─ 更新 receivedHex
            └─ 更新 chart 数据（chartDataVersion++）
                    ↓
            [QML视图] LineChartWidget 刷新曲线
```

---

## 四、协议帧格式

### 4.1 帧结构

```
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────────┬─────────┐
│  SOF    │  SOF1   │   CMD   │    ID   │   OP    │ v_len_L │ v_len_H │   Data      │ CRC16   │
│  0xAA   │  0x55   │  1字节  │  1字节  │  1字节  │  1字节  │  1字节  │  N字节      │  2字节  │
└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────────┴─────────┘
  ←─────────── Header (7字节) ───────────→  ←── Payload (N字节) ──→  ←── 小端序 ──→
```

> **注意**：v_len 和 CRC16 在 Windows 平台使用小端序存储，无需显式转换。

### 4.2 Payload 生成规则

| CMD | ID | Payload 长度 | 说明 |
|-----|----|-------------|------|
| 0x00 | 0x04, 0x06, 0x07, 0x08 | 1 字节 | byte[1] |
| 0x00 | 0x05 (0x10) | 16 字节 | byte[16] |
| 0x00 | 其他 | 0 字节 | 空 |
| 0x01 / 0x02 | 0x01, 0x03 | 1 字节 | byte[1] |
| 0x01 / 0x02 | 0x02 | 0 字节 | 空 |
| 0x03 | 所有 | 0 字节 | 空 |

---

## 五、编译与运行

### 5.1 环境要求

- Qt 6.7+（需包含 Qt SerialPort、Qt Charts、Qt Graphs 模块）
- CMake 3.24+
- C++17 编译器（MSVC 2022 / GCC 9+ / Clang 10+）

### 5.2 编译步骤

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

## 六、功能特性

- [x] 串口端口枚举与自动刷新
- [x] 串口参数配置（波特率、数据位、校验位、停止位、流控）
- [x] 数据收发与实时显示
- [x] 十六进制/ASCII 显示切换
- [x] 实时数据图表（加速度、角速度、姿态角、磁场）
- [x] 图表暂停/清除/导出功能
- [x] 图例点击隐藏/显示
- [x] **图表折叠功能**（点击箭头折叠，节省空间）
- [x] **多图表动态切换**（根据选中命令显示对应图表）
- [x] 多页面切换（数据显示、数据绘图、命令菜单、设置）
- [x] 页面状态控制（切换页面时停止渲染，避免卡顿）
- [x] 协议帧解析（支持多种数据类型）
- [x] CRC16 数据完整性校验
- [x] **独立解析线程**（不阻塞UI）
- [x] **命令面板**（指令分组、参数配置、HEX生成）
- [x] **接收数据过滤**（只显示匹配 cmd/id 的数据）
- [x] **结构化数据显示**（解析后格式化展示）
- [x] **统一 Button 组件**（4种类型 + hover/pressed/disabled 状态）
- [x] **亮/暗双主题**支持
- [x] 1000ms UI 刷新节流（避免高频刷新卡顿）

---

## 七、技术栈

| 模块 | 技术 |
|------|------|
| 框架 | Qt 6.7+ |
| 界面 | QML + Qt Quick Controls |
| 图表 | Qt Graphs (Qt6) |
| 串口 | Qt SerialPort |
| 构建 | CMake |
| 语言 | C++17 + QML |
| 架构 | MVVM |
| 线程 | QThread + 生产者消费者队列 |

---

## 八、扩展预留

- 蓝牙通讯服务
- WiFi 通讯服务
- MQTT 协议支持
- HTTP 接口支持

新增通讯服务仅需继承 `BaseCommService`，实现统一接口，上层调用逻辑无需改动。
