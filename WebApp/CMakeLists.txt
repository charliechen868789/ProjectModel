file(GLOB WEB_SRC  src/*.cpp)
add_executable(WebApp  ${WEB_SRC})
target_include_directories(WebApp  PRIVATE include ../Common/EventBus/include)
target_link_libraries(WebApp  PRIVATE EventBus)
set_target_properties(WebApp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)