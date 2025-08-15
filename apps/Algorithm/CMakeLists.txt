cmake_minimum_required(VERSION 3.16)

# 生成 Algorithm protobuf
set(ALGORITHM_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(ALGORITHM_PROTO_FILES
    ${ALGORITHM_PROTO_DIR}/algorithm_result.proto
)

set(ALGORITHM_PROTO_SRCS)
set(ALGORITHM_PROTO_HDRS)
foreach(proto_file ${ALGORITHM_PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)
    list(APPEND ALGORITHM_PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc)
    list(APPEND ALGORITHM_PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h)
endforeach()

add_custom_command(
    OUTPUT ${ALGORITHM_PROTO_SRCS} ${ALGORITHM_PROTO_HDRS}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${ALGORITHM_PROTO_DIR}
         ${ALGORITHM_PROTO_FILES}
    DEPENDS ${ALGORITHM_PROTO_FILES}
    COMMENT "Generating Algorithm protobuf files"
)

# 需要包含 VirtualSensor 的 proto 文件来解析传感器数据
set(SENSOR_PROTO_DIR ${CMAKE_SOURCE_DIR}/apps/VirtualSensor/proto)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sensor_data.pb.cc ${CMAKE_CURRENT_BINARY_DIR}/sensor_data.pb.h
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${SENSOR_PROTO_DIR}
         ${SENSOR_PROTO_DIR}/sensor_data.proto
    DEPENDS ${SENSOR_PROTO_DIR}/sensor_data.proto
    COMMENT "Generating sensor_data protobuf for Algorithm"
)

add_executable(Algorithm
    src/main.cpp
    src/Algorithm.cpp
    ${ALGORITHM_PROTO_SRCS}
    ${CMAKE_CURRENT_BINARY_DIR}/sensor_data.pb.cc
)

target_include_directories(Algorithm PRIVATE
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(Algorithm PRIVATE
    AppTemplate
    ${Protobuf_LIBRARIES}
)