cmake_minimum_required(VERSION 3.16)

# 生成 EventBus protobuf
set(EVENTBUS_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(EVENTBUS_PROTO_FILES
    ${EVENTBUS_PROTO_DIR}/event_message.proto
)

set(EVENTBUS_PROTO_SRCS)
set(EVENTBUS_PROTO_HDRS)
foreach(proto_file ${EVENTBUS_PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)
    list(APPEND EVENTBUS_PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc)
    list(APPEND EVENTBUS_PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h)
endforeach()

add_custom_command(
    OUTPUT ${EVENTBUS_PROTO_SRCS} ${EVENTBUS_PROTO_HDRS}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         --proto_path=${EVENTBUS_PROTO_DIR}
         ${EVENTBUS_PROTO_FILES}
    DEPENDS ${EVENTBUS_PROTO_FILES}
    COMMENT "Generating EventBus protobuf files"
)

# EventBus 共享库
add_library(EventBus SHARED
    src/EventBus.cpp
    src/TcpServer.cpp
    src/TcpClient.cpp
    ${EVENTBUS_PROTO_SRCS}
)

target_include_directories(EventBus PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(EventBus PUBLIC
    ${Protobuf_LIBRARIES}
    Threads::Threads
)