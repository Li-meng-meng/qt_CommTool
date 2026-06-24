# CommTool - 串口调试工具（Model抽离独立工具库·第一版仅串口）
一款**跨平台、可扩展、规范化**的通讯调试工具，第一版仅实现串口通讯，蓝牙/WiFi/MQTT/HTTP等通讯服务预留扩展，Model层拆分为独立可复用工具库 `CommModelLib`，补充完整 **CommModelLib 接口文档** + **独立库单元测试** 完整章节。

---

## 一、项目结构
核心改动说明：
1. 新增独立库目录 `CommModelLib`，所有通讯底层模型抽离为静态库，主程序仅依赖该库；
2. 第一版仅保留串口相关代码，蓝牙、WiFi、MQTT、HTTP目录仅做文件夹占位，无实现文件；
3. 主工程 `CommTool` 只负责UI、ViewModel、通用工具，业务通讯能力全部外包给独立Model库；
4. 单元测试拆分：通用模块测试、独立模型库专项单元测试；
5. 新增 `CommModelLib/docs` 存放库对外接口说明文档。

```
CommTool/
├── CommModelLib/              # 独立通讯模型工具库（静态库，可单独编译复用）
│   ├── docs/                  # 【新增】CommModelLib 对外接口说明文档
│   │   └── SerialService_Interface.md
│   ├── Common/                # 库内部通用枚举、基类、工具
│   │   ├── EnumComm.h         # 通讯枚举、连接状态、串口参数枚举
│   │   ├── Singleton.h        # 单例模板
│   │   └── BaseCommService.cpp/h # 通讯服务抽象基类
│   ├── SerialPort/            # 第一版唯一实现：串口服务
│   │   ├── SerialService.cpp
│   │   └── SerialService.h
│   ├── Bluetooth/             # 预留扩展目录（第一版无代码）
│   ├── Http/                  # 预留扩展目录（第一版无代码）
│   ├── MQTT/                  # 预留扩展目录（第一版无代码）
│   └── WiFi/                  # 预留扩展目录（第一版无代码）
│
├── Common/                    # 主程序通用工具模块（UI层共用）
│   ├── LogManager.cpp/h      # 日志管理器（单例模式）
│   └── UtilsCommon.cpp/h     # UI通用工具函数
│
├── Help/                      # 帮助模块
│   └── HelpView.qml          # 帮助对话框（Dialog类型）
│
├── View/                     # 视图层（QML界面）
│   ├── Component/            # 自定义组件
│   │   └── Button.qml        # 统一按钮组件
│   ├── DataShowView.qml      # 数据收发显示视图
│   ├── MainView.qml          # 主界面视图
│   ├── SettingView.qml       # 设置界面视图（仅串口参数配置）
│   └── SideBarView.qml       # 侧边栏菜单视图（仅串口入口）
│
├── ViewModel/                # 视图模型层（MVVM）
│   ├── CommViewModel.cpp/h   # 串口通讯视图模型，依赖CommModelLib
│   ├── MainViewModel.cpp/h   # 主界面视图模型
│   └── SettingViewModel.cpp/h# 串口参数设置视图模型
│
├── cmake/                    # CMake辅助配置
│   ├── QtBuildHelpers.cmake
│   └── ModelLibConfig.cmake  # 独立模型库编译链接配置
│
├── tests/                    # 单元测试
│   ├── Common/               # 主程序通用模块测试
│   └── CommModelLibTest/     # 【完善】独立模型库专项单元测试（仅串口）
│       ├── CMakeLists.txt    # 测试子工程编译脚本
│       ├── TestBaseService.cpp
│       ├── TestSerialService.cpp # 串口服务全接口单元测试
│       └── TestEnumComm.cpp
│
├── CMakeLists.txt            # 顶层CMake，先编译CommModelLib再编译主程序
├── main.cpp                  # 应用入口
├── qml.qrc                   # QML资源文件
└── .gitignore                # Git忽略配置
```

**模块职责说明：**

| 层级 | 职责 | 技术实现 |
|------|------|----------|
| **View** | 界面渲染、用户交互、串口参数配置、收发数据展示 | QML + Qt Quick Controls |
| **ViewModel** | 数据转换、UI与底层库协调、参数透传、状态转发 | C++ QObject，依赖CommModelLib对外接口 |
| **CommModelLib(独立库)** | 底层串口核心业务逻辑、串口硬件操作、收发数据处理、通讯抽象基类；对外提供标准化接口文档 | C++ 静态库，可脱离主程序单独复用 |
| **Common(主程序)** | UI层工具、日志管理、界面辅助函数 | C++ 模板/单例 |
| **CommModelLib/docs** | 库对外接口规范、函数入参出参、信号说明、调用示例 | Markdown 接口文档，供上层ViewModel/外部项目查阅 |
| **tests/CommModelLibTest** | 覆盖库全部对外接口，边界用例、异常场景、正常流程自动化测试 | Qt Test 单元测试框架 |

---

## 二、CommModelLib 接口说明文档（SerialService_Interface.md 完整内容）
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
```cpp
// 获取当前连接状态
virtual CommConnectState getConnectState() const = 0;
// 关闭通讯端口
virtual void closePort() = 0;
// 发送字节数据
virtual bool sendData(const QByteArray &data) = 0;
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

---

## 三、CommModelLib 单元测试模块说明（tests/CommModelLibTest）
### 3.1 测试框架
基于 Qt Test 实现独立测试子工程，可单独编译运行，不依赖主程序UI。
### 3.2 测试文件划分
1. `TestEnumComm.cpp`：枚举类型转换、参数映射测试
2. `TestBaseService.cpp`：抽象基类接口规范校验
3. `TestSerialService.cpp`：串口服务完整接口自动化测试（核心）

### 3.3 TestSerialService 覆盖测试用例
#### （1）端口枚举测试
- 正常获取本机串口列表
- 无可用串口边界场景
- 多次刷新端口列表一致性校验

#### （2）参数配置测试
- 波特率、数据位、校验位合法参数设置读取校验
- 非法空串口名参数容错测试
- 参数批量设置与读取一致性

#### （3）端口打开/关闭流程
- 正常端口打开成功流程
- 重复打开同一端口异常处理
- 打开不存在端口失败校验
- 打开后正常关闭、重复关闭容错

#### （4）数据收发测试
- 端口未打开调用sendData返回false
- 正常连接下发送字节数据
- 空数据发送容错
- 底层接收数据信号触发校验

#### （5）状态与异常信号测试
- 打开/关闭对应状态机切换
- 端口占用、权限不足错误信号捕获
- 所有信号触发时机、参数正确性校验

### 3.4 CommModelLibTest CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.24)
project(CommModelLibTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core SerialPort Test)

qt_standard_project_setup()

qt_add_executable(CommModelLibTest
    TestEnumComm.cpp
    TestBaseService.cpp
    TestSerialService.cpp
)

# 链接独立模型库 + Qt测试模块
target_link_libraries(CommModelLibTest
    PRIVATE
        Qt6::Core
        Qt6::SerialPort
        Qt6::Test
        CommModelLib
)

install(TARGETS CommModelLibTest BUNDLE DESTINATION .)
```

### 3.5 执行测试命令
```bash
# 编译主工程后进入build目录
cd build/tests/CommModelLibTest
./CommModelLibTest

# 输出xml测试报告
./CommModelLibTest -xml test_result.xml
```

### 3.6 测试分层价值
1. 独立库无UI依赖，可CI自动化编译测试；
2. 所有底层接口变更必须同步更新单元用例，提前拦截串口逻辑bug；
3. 接口文档与单元测试用例一一对应，上层开发可对照文档+测试代码理解调用逻辑。

---

## 四、原有章节精简保留（仅保留改动说明，其余不变）
### 二、用到的知识点（补充库文档&单元测试相关）
1. Qt Test 单元测试框架，独立子工程测试静态库
2. 库接口文档标准化Markdown维护，前后端开发对齐
3. CMake多子项目管理：业务库、主程序、测试程序三分离

### 三、ViewModel 与 View、独立Model库交互方式
不变，仅补充一句：**所有调用逻辑可参考 CommModelLib/docs 接口文档 + tests/CommModelLibTest 示例代码**

### 四、跨平台宏、编译、许可证章节完全不变

---

## 五、新增关键修改汇总
1. **补充 CommModelLib 接口文档目录**：`CommModelLib/docs/SerialService_Interface.md`，完整定义所有对外函数、信号、枚举、调用示例、异常规范；
2. **完善独立库单元测试模块**：`tests/CommModelLibTest` 提供完整CMake编译脚本，全覆盖串口服务全部接口、边界、异常场景；
3. 文档与测试双向绑定：接口文档提供调用规范，单元测试提供可运行参考代码；
4. 测试完全解耦UI：仅依赖CommModelLib静态库，可单独编译、自动化CI测试；
5. 项目结构、职责表新增文档、测试模块说明，明确维护规范。

---

## 六、通讯数据流程解析（从接收到解析完整链路）

### 6.1 数据流程总览
```
串口硬件接收 → SerialService → CommViewModel → DataParser → CommImpl → 回调解析 → 数据展示
```

### 6.2 各层函数与信号槽详细流程

#### （1）SerialService 层（底层串口）
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 信号 | `sigRecvData(const QByteArray&)` | 底层接收到串口数据后发出 | `CommModelLib/SerialPort/SerialService.h` |

#### （2）CommViewModel 层（数据转发与队列管理）
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 槽函数 | `onRecvData(const QByteArray& data)` | 接收 `sigRecvData` 信号，将数据入队 | `ViewModel/CommViewModel.cpp:324` |
| 函数 | `enqueueData(const QByteArray& data)` | 使用互斥锁将数据加入 `m_dataQueue` 队列 | `ViewModel/CommViewModel.cpp:340` |
| 函数 | `processDataQueue()` | 定时器定时处理队列，合并数据后转发 | `ViewModel/CommViewModel.cpp:346` |
| 调用 | `m_dataParser->parse(data)` | 将数据传给 DataParser 进行解析 | `ViewModel/CommViewModel.cpp:377` |

**信号连接关系**：
```cpp
connect(&m_serial, &SerialService::sigRecvData,
        this, &CommViewModel::onRecvData);
```

#### （3）DataParser 层（协议解析与分发）
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 函数 | `parse(const QByteArray& rawData)` | 解析入口，调用 CommImpl 状态机 | `ViewModel/DataParser.cpp:21` |
| 槽函数 | `onFrameParsed(const QByteArray& frame)` | 接收完整帧后调用 `user_unpack_data_fun` | `ViewModel/DataParser.cpp:31` |
| 函数 | `user_unpack_data_fun(const QByteArray& buf, quint16 len)` | 根据帧头 cmd/id 匹配回调函数 | `ViewModel/DataParser.cpp:36` |
| 信号 | `newAccelData(const AccelerationData&)` | 解析出加速度数据后发出 | `ViewModel/DataParser.h:156` |
| 信号 | `newGyroData(const GyroscopeData&)` | 解析出角速度数据后发出 | `ViewModel/DataParser.h:157` |
| 信号 | `newSensorData(const SensorData&)` | 解析出完整传感器数据后发出 | `ViewModel/DataParser.h:159` |
| 信号 | `parseError(const QString&)` | 解析错误时发出 | `ViewModel/DataParser.h:162` |

**信号连接关系**：
```cpp
connect(m_commImpl, &CommImpl::frameParsed, this, &DataParser::onFrameParsed);
connect(m_commImpl, &CommImpl::parseError, this, &DataParser::parseError);
```

#### （4）CommImpl 层（状态机帧解析）
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 函数 | `parse(const QByteArray& rawData)` | 状态机解析入口，逐字节处理 | `ViewModel/CommImpl.cpp:13` |
| 信号 | `frameParsed(const QByteArray& frame)` | 解析出完整帧后发出 | `ViewModel/CommImpl.h:31` |
| 信号 | `parseError(const QString& error)` | 帧解析失败时发出 | `ViewModel/CommImpl.h:32` |
| 函数 | `Reset()` | 重置解析状态机 | `ViewModel/CommImpl.cpp:112` |
| 函数 | `IsOverflow()` | 检查缓冲区溢出 | `ViewModel/CommImpl.cpp:119` |
| 模板函数 | `ByteArrayToStructure<T>(const QByteArray&)` | 将字节数组转换为结构体 | `ViewModel/CommImpl.h:20` |

**状态机解析流程**：
```
Step 0: 等待帧头 0xAA
    ↓ (收到 0xAA)
Step 1: 等待帧头 0x55
    ↓ (收到 0x55)
Step 2: 接收 cmd 字节
    ↓
Step 3: 接收 id 字节
    ↓
Step 4: 接收 v_len 低字节
    ↓
Step 5: 接收 v_len 高字节，计算包长度
    ↓
Step 6: 接收数据内容，直到达到包长度
    ↓
CRC16 校验 → 通过 → 发出 frameParsed 信号
          ↓ 失败
       发出 parseError 信号
```

#### （5）CRC 工具类（数据完整性校验）
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 静态函数 | `CRC16(const quint8* puchMsg, quint16 usDataLen)` | 计算 CRC16 校验值 | `ViewModel/crc.cpp` |

#### （6）回调函数词典（协议分发）
DataParser 使用回调函数词典模式根据 cmd/id 分发解析：

| 回调函数 | cmd | id | 解析数据类型 | 文件位置 |
|----------|-----|----|--------------|----------|
| `cmd0_id1_get_data` | 0x00 | 0x01 | SensorDataPacket（加速度、角速度等） | `ViewModel/DataParser.cpp:54` |
| `cmd0_id2_get_data` | 0x00 | 0x02 | StatusPacket（状态数据） | `ViewModel/DataParser.cpp:78` |
| `cmd0_id3_get_data` | 0x00 | 0x03 | VisonPacket（视觉数据） | `ViewModel/DataParser.cpp:89` |
| `cmd0_id4_get_data` | 0x00 | 0x04 | HzPacket（频率数据） | `ViewModel/DataParser.cpp:100` |
| `cmd0_id5_get_data` | 0x00 | 0x05 | SnPacket（序列号数据） | `ViewModel/DataParser.cpp:111` |
| `cmd0_id6_get_data` | 0x00 | 0x06 | AlgorithmPacket（算法数据） | `ViewModel/DataParser.cpp:122` |
| `cmd0_id7_get_data` | 0x00 | 0x07 | APacket | `ViewModel/DataParser.cpp:133` |
| `cmd0_id8_get_data` | 0x00 | 0x08 | APacket | `ViewModel/DataParser.cpp:144` |
| `cmd0_id9_get_data` | 0x00 | 0x09 | CalibrationData（校准数据） | `ViewModel/DataParser.cpp:155` |
| `cmd0_id10_get_data` | 0x00 | 0x0A | SensorData（传感器数据） | `ViewModel/DataParser.cpp:166` |
| `cmd1_id1_get_data` | 0x01 | 0x01 | APacket | `ViewModel/DataParser.cpp:198` |
| `cmd1_id2_get_data` | 0x01 | 0x02 | AllDataPacket（全量数据） | `ViewModel/DataParser.cpp:208` |
| `cmd1_id3_get_data` | 0x01 | 0x03 | PowerPacket（电源数据） | `ViewModel/DataParser.cpp:233` |

**回调函数词典定义**：
```cpp
const DataParser::user_comm_callbak_t DataParser::user_comm_callbak[] = {
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id1), cmd0_id1_get_data },
    { static_cast<quint8>(Cmd::Cmd0), static_cast<quint8>(Id::Id2), cmd0_id2_get_data },
    // ... 其他回调函数
};
```

#### （7）CommViewModel → DataPlotViewModel 连接
| 信号 | 槽函数 | 说明 | 文件位置 |
|------|--------|------|----------|
| `DataParser::newAccelData` | `CommViewModel::onNewAccelData` | 接收加速度数据 | `ViewModel/CommViewModel.cpp` |
| `DataParser::newGyroData` | `CommViewModel::onNewGyroData` | 接收角速度数据 | `ViewModel/CommViewModel.cpp` |
| `DataParser::newSensorData` | `CommViewModel::onNewSensorData` | 接收完整传感器数据 | `ViewModel/CommViewModel.cpp:503` |
| `DataParser::parseError` | `CommViewModel::onParseError` | 接收解析错误 | `ViewModel/CommViewModel.cpp:513` |

**信号连接关系**：
```cpp
connect(m_dataParser, &DataParser::newAccelData,
        this, &CommViewModel::onNewAccelData);
connect(m_dataParser, &DataParser::newGyroData,
        this, &CommViewModel::onNewGyroData);
connect(m_dataParser, &DataParser::newSensorData,
        this, &CommViewModel::onNewSensorData);
connect(m_dataParser, &DataParser::parseError,
        this, &CommViewModel::onParseError);
```

#### （8）DataPlotViewModel 层（图表数据处理）
| 类型 | 名称 | 说明 | 文件位置 |
|------|------|------|----------|
| 函数 | `addDataPoint(qint64 timestamp, double ax, double ay, double az, double gx, double gy, double gz)` | 添加数据点到图表 | `ViewModel/DataPlotViewModel.cpp` |

### 6.3 完整数据流转时序图

```
[串口硬件] → SerialService::sigRecvData(rawData)
                ↓
[CommViewModel] → onRecvData(rawData)
                ↓
                enqueueData(rawData) → m_dataQueue
                ↓
                processDataQueue() [定时器触发]
                ↓
                m_dataParser->parse(mergedData)
                ↓
[DataParser] → parse(rawData) → m_commImpl->parse(rawData)
                ↓
[CommImpl] → parse(rawData) [状态机逐字节解析]
                ↓ (帧解析完成)
                frameParsed(frame) [信号]
                ↓
[DataParser] → onFrameParsed(frame)
                ↓
                user_unpack_data_fun(frame, len)
                ↓ (匹配到 cmd/id)
                cmd0_id1_get_data(frame, len) [回调函数]
                ↓
                newAccelData(accel) [信号]
                newGyroData(gyro) [信号]
                ↓
[CommViewModel] → onNewAccelData(accel)
                → onNewGyroData(gyro)
                ↓
                m_dataPlotViewModel->addDataPoint(...)
                ↓
[DataPlotViewModel] → 更新图表数据
                ↓
[QML视图] → LineChartWidget 刷新显示曲线
```

### 6.4 协议帧结构
```
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────────┬─────────┐
│  SOF    │  SOF1   │   CMD   │    ID   │ v_len_L │ v_len_H │   Data      │ CRC16   │
│  0xAA   │  0x55   │  1字节  │  1字节  │  1字节  │  1字节  │  N字节      │  2字节  │
└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────────┴─────────┘
```

- **SOF/SOF1**：帧头标识，固定为 `0xAA 0x55`
- **CMD**：命令类型，决定数据结构（0x00-0x03）
- **ID**：命令子类型，进一步区分数据内容（0x01-0x0A）
- **v_len**：数据长度（小端序，2字节）
- **Data**：实际数据内容，长度由 v_len 指定
- **CRC16**：校验值，覆盖从 SOF 到 Data 末尾
