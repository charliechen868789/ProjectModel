file(GLOB GUI_SRC src/*.cpp)
add_executable(Gui ${GUI_SRC})
target_include_directories(Gui PRIVATE include     
    ../Common/EventBus/include
    ../Common/AppTemplate/include)
target_link_libraries(Gui PRIVATE EventBus AppTemplate)
set_target_properties(Gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
