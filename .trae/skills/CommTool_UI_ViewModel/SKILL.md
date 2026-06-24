---
name: CommModelLib_UnitTest
description: 为CommModelLib编写完整Qt Test自动化测试，覆盖全部公开接口、异常场景和边界条件。
---

# Purpose

测试：

CommModelLib

只测试：

```text

EnumComm

BaseCommService

SerialService

```

使用：

```text

Qt Test

QSignalSpy

```

禁止：

UI

QML

MainWindow

ViewModel

# Test Files

必须：

```text

TestEnumComm.cpp

TestBaseService.cpp

TestSerialService.cpp

```

# TestEnumComm

测试：

```text

BaudRate

DataBits

Parity

StopBits

FlowControl

CommConnectState

```

验证：

```text

枚举值

转换关系

默认值

```

# TestSerialService

必须覆盖：

## PortList

测试：

```text

正常获取串口列表

无串口

重复刷新

```

## Open

测试：

```text

正常打开

不存在端口

端口占用

重复打开

空端口

```

## Close

测试：

```text

正常关闭

重复关闭

关闭后状态

```

## Send

测试：

```text

发送成功

空数据

未打开发送

超长数据

```

## Receive

测试：

```text

收到数据

多次收到

大数据

```

验证：

```cpp

QSignalSpy spy(

&m_serial,

&SerialService::sigRecvData

);

```

# Error

必须验证：

```cpp

sigErrorOccurred

```

错误：

```text

端口不存在

发送失败

打开失败

权限错误

```

# Coverage

目标：

```text

Statement:

>=90%

Branch:

>=85%

```

# Output

输出：

完整：

TestEnumComm.cpp

TestBaseService.cpp

TestSerialService.cpp

tests/CMakeLists.txt

全部可直接编译运行。