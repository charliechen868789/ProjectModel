cmake_minimum_required(VERSION 3.16)

# WebApp 程序需要 Algorithm 和 VirtualSensor 的 proto 文件
set(ALGORITHM_PROTO_DIR ${CMAKE_SOURCE_DIR}/apps/Algorithm/proto)
set(SENSOR_PROTO_DIR ${CMAKE_SOURCE_DIR}/apps/VirtualSensor/proto)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/algorithm_result.pb.cc ${CMAKE_CURRENT_BINARY_DIR}/algorithm_result.pb.h
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${ALGORITHM_PROTO_DIR}
         ${ALGORITHM_PROTO_DIR}/algorithm_result.proto
    DEPENDS ${ALGORITHM_PROTO_DIR}/algorithm_result.proto
    COMMENT "Generating algorithm_result protobuf for WebApp"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sensor_data.pb.cc ${CMAKE_CURRENT_BINARY_DIR}/sensor_data.pb.h
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${SENSOR_PROTO_DIR}
         ${SENSOR_PROTO_DIR}/sensor_data.proto
    DEPENDS ${SENSOR_PROTO_DIR}/sensor_data.proto
    COMMENT "Generating sensor_data protobuf for WebApp"
)

add_executable(WebApp
    src/main.cpp
    src/WebApp.cpp
    src/HttpServer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/algorithm_result.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/sensor_data.pb.cc
)

target_include_directories(WebApp PRIVATE
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(WebApp PRIVATE
    AppTemplate
    ${Protobuf_LIBRARIES}
)