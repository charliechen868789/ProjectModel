# 项目根目录 CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(EventDrivenTemplate VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找依赖
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)   # if you use Qt resource files
set(CMAKE_AUTOUIC ON)   # if you use .ui files

# 包含子目录
add_subdirectory(libs/EventBus)
add_subdirectory(libs/AppTemplate)
add_subdirectory(libs/ConfigManager)
add_subdirectory(apps/VirtualSensor)
add_subdirectory(apps/Algorithm)
add_subdirectory(apps/GUI)
add_subdirectory(apps/WebApp)