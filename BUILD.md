################################################################################
# BUILD.md - 构建说明
################################################################################

# Event-Driven Template Project

这是一个完整的事件驱动C++模板工程，包含共享库和四个独立应用程序。

## 项目结构

```
EventDrivenTemplate/
├── CMakeLists.txt              # 根CMake配置
├── BUILD.md                    # 构建说明
├── README.md                   # 项目说明
├── libs/                       # 共享库
│   ├── EventBus/              # 事件总线库
│   │   ├── CMakeLists.txt
│   │   ├── include/
│   │   │   ├── EventBus.h
│   │   │   ├── TcpServer.h
│   │   │   └── TcpClient.h
│   │   ├── src/
│   │   │   ├── EventBus.cpp
│   │   │   ├── TcpServer.cpp
│   │   │   └── TcpClient.cpp
│   │   └── proto/
│   │       └── event_message.proto
│   └── AppTemplate/           # 应用模板库
│       ├── CMakeLists.txt
│       ├── include/
│       │   └── AppTemplate.h
│       └── src/
│           └── AppTemplate.cpp
└── apps/                      # 应用程序
    ├── VirtualSensor/         # 虚拟传感器
    │   ├── CMakeLists.txt
    │   ├── include/
    │   │   └── VirtualSensor.h
    │   ├── src/
    │   │   ├── main.cpp
    │   │   └── VirtualSensor.cpp
    │   └── proto/
    │       └── sensor_data.proto
    ├── Algorithm/             # 算法处理器
    │   ├── CMakeLists.txt
    │   ├── include/
    │   │   └── Algorithm.h
    │   ├── src/
    │   │   ├── main.cpp
    │   │   └── Algorithm.cpp
    │   └── proto/
    │       └── algorithm_result.proto
    ├── GUI/                   # 命令行界面
    │   ├── CMakeLists.txt
    │   ├── include/
    │   │   └── GUI.h
    │   └── src/
    │       ├── main.cpp
    │       └── GUI.cpp
    └── WebApp/               # Web界面
        ├── CMakeLists.txt
        ├── include/
        │   ├── WebApp.h
        │   └── HttpServer.h
        └── src/
            ├── main.cpp
            ├── WebApp.cpp
            └── HttpServer.cpp
```

## 依赖要求

- CMake 3.16+
- C++17编译器 (GCC 7+ 或 Clang 5+)
- Protocol Buffers (libprotobuf-dev)
- pthread

### Ubuntu/Debian 安装依赖
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libprotobuf-dev protobuf-compiler
```

### CentOS/RHEL 安装依赖
```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake3 protobuf-devel protobuf-compiler
```

### macOS 安装依赖
```bash
brew install cmake protobuf
```

## 构建步骤

1. 克隆项目并创建构建目录
```bash
git clone <repository-url>
cd EventDrivenTemplate
mkdir build && cd build
```

2. 配置和构建
```bash
cmake ..
make -j$(nproc)
```

3. 构建结果
所有可执行文件将输出到 `build/bin/` 目录：
- `VirtualSensor` - 虚拟传感器程序
- `Algorithm` - 算法处理程序  
- `GUI` - 命令行界面程序
- `WebApp` - Web服务器程序

共享库将输出到 `build/lib/` 目录：
- `libEventBus.so` - 事件总线库
- `libAppTemplate.so` - 应用模板库

## 运行应用

### 方法1: 按顺序启动各应用
```bash
cd build/bin

# 终端1: 启动虚拟传感器
./VirtualSensor

# 终端2: 启动算法处理器
./Algorithm

# 终端3: 启动GUI界面
./GUI

# 终端4: 启动Web应用
./WebApp
```

### 方法2: 使用启动脚本
```bash
#!/bin/bash
# start_all.sh

cd build/bin

# 后台启动各应用
./VirtualSensor &
sleep 2
./Algorithm &
sleep 2
./WebApp &
sleep 2

# 前台运行GUI
./GUI
```

## 使用说明

### VirtualSensor (虚拟传感器)
- 端口: 20001
- 功能: 每2秒生成模拟温度、湿度、气压数据并广播

### Algorithm (算法处理器)  
- 端口: 20002
- 功能: 接收传感器数据，计算舒适度指数，生成警报和建议

### GUI (命令行界面)
- 端口: 20003  
- 功能: 显示实时传感器数据和算法结果
- 命令: r(刷新), s(状态), q(退出)

### WebApp (Web界面)
- 端口: 20004 (EventBus), 8080 (HTTP)
- 功能: 提供Web界面显示数据
- 访问: http://localhost:8080

## 事件机制

### EventBus 功能
- `postEvent()`: 本地事件分发
- `broadcast()`: 跨进程事件广播  
- `registerHandler()`: 注册事件处理函数
- TCP自动连接和重连

### 事件类型
- `sensor.data`: 传感器数据事件
- `algorithm.result`: 算法结果事件

## 独立构建

每个库和应用都可以独立构建：

```bash
# 仅构建EventBus库
cmake --build . --target EventBus

# 仅构建VirtualSensor应用
cmake --build . --target VirtualSensor

# 仅构建特定目标
cmake --build . --target <target_name>
```

## 故障排除

### 端口冲突
如果端口被占用，可以修改各应用的端口配置：
- VirtualSensor: 修改 `VirtualSensor.cpp` 中的端口号
- Algorithm: 修改 `Algorithm.cpp` 中的端口号  
- GUI: 修改 `GUI.cpp` 中的端口号
- WebApp: 修改 `WebApp.cpp` 中的端口号

### Protobuf错误
确保安装了正确版本的protobuf：
```bash
protoc --version  # 应该显示版本号
```

### 权限问题
如果遇到权限问题，确保用户有执行权限：
```bash
chmod +x build/bin/*
```

################################################################################
# README.md - 项目说明
################################################################################

# Event-Driven Template Project

一个基于事件驱动架构的C++模板工程，展示了如何构建可扩展的多进程应用系统。

## 特性

- 🚀 **事件驱动架构**: 基于EventBus的本地和跨进程事件系统
- 🔧 **模块化设计**: 共享库和独立应用程序分离
- 🌐 **多界面支持**: 命令行GUI和Web界面
- 📡 **实时通信**: TCP跨进程通信，支持自动重连
- 🏗️ **现代C++**: C++17标准，CMake构建系统
- 📋 **Protocol Buffers**: 高效的序列化协议

## 系统架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  VirtualSensor  │───▶│   Algorithm     │───▶│      GUI        │
│   (生成数据)     │    │   (处理数据)     │    │   (显示数据)     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                 ┌─────────────────▼─────────────────┐
                 │            WebApp               │
                 │         (Web界面)               │
                 └─────────────────────────────────┘
```

## 核心组件

### 共享库

#### EventBus
- TCP跨进程事件广播
- 本地事件分发
- 自动连接管理
- 线程安全

#### AppTemplate  
- 统一的应用程序接口
- 封装EventBus功能
- 标准化的应用生命周期

### 应用程序

#### VirtualSensor
- 生成模拟传感器数据(温度、湿度、气压)
- 定时广播数据事件
- 可配置的传感器参数

#### Algorithm
- 接收传感器数据
- 计算环境舒适度指数
- 生成警报级别和建议

#### GUI
- 命令行界面显示
- 实时数据更新
- 交互式命令系统

#### WebApp
- HTTP服务器
- 响应式Web界面
- REST API接口

## 事件流程

1. **VirtualSensor** 生成传感器数据 → broadcast("sensor.data")
2. **Algorithm** 接收数据 → 处理计算 → broadcast("algorithm.result")  
3. **GUI/WebApp** 接收结果 → 更新显示

## 快速开始

详细构建和运行说明请参考 [BUILD.md](BUILD.md)

```bash
# 构建项目
mkdir build && cd build
cmake .. && make -j$(nproc)

# 运行所有应用
cd bin
./VirtualSensor &
./Algorithm &  
./WebApp &
./GUI
```

访问Web界面: http://localhost:8080

## 扩展指南

### 添加新应用
1. 在 `apps/` 目录创建新的应用目录
2. 编写CMakeLists.txt配置
3. 继承AppTemplate类
4. 实现initialize(), run(), cleanup()方法
5. 注册所需的事件处理器

### 添加新事件类型
1. 定义新的protobuf消息格式
2. 在相应应用中注册事件处理器
3. 使用broadcast()发送事件

### 自定义EventBus
EventBus支持灵活配置：
- 自定义端口号
- 添加事件过滤器
- 实现自定义序列化协议

## 技术细节

- **通信协议**: TCP + Protocol Buffers
- **并发模型**: 多线程 + 事件驱动
- **错误处理**: 异常安全 + 自动重连
- **内存管理**: RAII + 智能指针

## License

MIT License - 详见LICENSE文件
