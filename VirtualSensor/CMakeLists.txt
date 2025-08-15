file(GLOB SENSOR_SRC  src/*.cpp)
add_executable(VirtualSensor ${SENSOR_SRC})
target_include_directories(VirtualSensor PRIVATE
    ../Common/EventBus/include
    ../Common/AppTemplate/include)
target_link_libraries(VirtualSensor PRIVATE EventBus AppTemplate)
set_target_properties(VirtualSensor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)