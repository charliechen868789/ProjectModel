file(GLOB SENSOR_SRC  src/*.cpp)
add_executable(VirtualSensor ${SENSOR_SRC})
target_include_directories(VirtualSensor PRIVATE include ../Common/EventBus/include)
target_link_libraries(VirtualSensor PRIVATE EventBus)
set_target_properties(VirtualSensor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)