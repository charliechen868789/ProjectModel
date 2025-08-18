# CMakeLists.txt for Qt GUI Application
cmake_minimum_required(VERSION 3.16)
project(QtEnvironmentMonitor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 Network HttpServer)

# Protobuf
find_package(Protobuf REQUIRED)


# 生成 VirtualSensor protobuf
set(VIRTUALSENSOR_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../VirtualSensor/proto)
set(VIRTUALSENSOR_PROTO_FILES
    ${VIRTUALSENSOR_PROTO_DIR}/sensor_data.proto
)

set(VIRTUALSENSOR_PROTO_SRCS)
set(VIRTUALSENSOR_PROTO_HDRS)
foreach(proto_file ${VIRTUALSENSOR_PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)
    list(APPEND VIRTUALSENSOR_PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc)
    list(APPEND VIRTUALSENSOR_PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h)
endforeach()

add_custom_command(
    OUTPUT ${VIRTUALSENSOR_PROTO_SRCS} ${VIRTUALSENSOR_PROTO_HDRS}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${VIRTUALSENSOR_PROTO_DIR}
         ${VIRTUALSENSOR_PROTO_FILES}
    DEPENDS ${VIRTUALSENSOR_PROTO_FILES}
    COMMENT "Generating VirtualSensor protobuf files"
)

# Create a target to generate protobuf files
add_custom_target(GenerateVirtualSensorProtobuf ALL
    DEPENDS ${VIRTUALSENSOR_PROTO_SRCS} ${VIRTUALSENSOR_PROTO_HDRS}
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ 
    ../../libs/AppTemplate/include
    ../../libs/EventBus/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/model/
     ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS})

# Qt GUI Application
qt_add_resources(GUI_RESOURCES appqml.qrc)
qt6_add_executable(QtEnvironmentMonitor
    src/main.cpp
    src/QtGUI.cpp
    include/QtGUI.h
    model/DataModel.cpp
    model/DataModel.h
    src/LayoutManager.cpp
    include/LayoutManager.h
    ${GUI_RESOURCES}
    ${VIRTUALSENSOR_PROTO_SRCS}
)

# Make the executable depend on protobuf generation
add_dependencies(QtEnvironmentMonitor GenerateVirtualSensorProtobuf)

# ----------------------
# Qt QML Module
# ----------------------
qt6_add_qml_module(QtEnvironmentMonitor
    URI EnvironmentMonitor
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/SensorPanel.qml
        qml/AlgorithmPanel.qml
        qml/ChartPanel.qml
        qml/StatusBar.qml
        qml/DynamicLayout.qml
)

# Keyword-style target_link_libraries
target_link_libraries(QtEnvironmentMonitor
    PRIVATE Qt6::Core
    PRIVATE Qt6::Quick
    PRIVATE Qt6::QuickControls2
    PRIVATE Qt6::Network
    PRIVATE protobuf::libprotobuf
    PRIVATE EventBus
    PRIVATE AppTemplate
)