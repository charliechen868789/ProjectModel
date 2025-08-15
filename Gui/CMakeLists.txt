file(GLOB GUI_SRC src/*.cpp)
add_executable(Gui ${GUI_SRC})
target_include_directories(Gui PRIVATE include ../Common/EventBus/include)
target_link_libraries(Gui PRIVATE EventBus)
set_target_properties(Gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
