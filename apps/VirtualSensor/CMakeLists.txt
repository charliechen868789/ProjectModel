cmake_minimum_required(VERSION 3.16)

# 生成 VirtualSensor protobuf
set(VIRTUALSENSOR_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
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

add_executable(VirtualSensor
    src/main.cpp
    src/VirtualSensor.cpp
    ${VIRTUALSENSOR_PROTO_SRCS}
)

target_include_directories(VirtualSensor PRIVATE
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(VirtualSensor PRIVATE
    AppTemplate
    ${Protobuf_LIBRARIES}
)