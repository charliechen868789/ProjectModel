file(GLOB ALGO_SRC src/*.cpp)
add_executable(Algorithm ${ALGO_SRC})
target_include_directories(Algorithm PRIVATE include 
    ../Common/EventBus/include
    ../Common/AppTemplate/include)
target_link_libraries(Algorithm PRIVATE EventBus AppTemplate)
set_target_properties(Algorithm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)