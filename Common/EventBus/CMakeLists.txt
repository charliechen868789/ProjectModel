file(GLOB EVENTBUS_SRC src/*.cpp)
add_library(EventBus SHARED ${EVENTBUS_SRC})
target_include_directories(EventBus PUBLIC include)
target_link_libraries(EventBus PUBLIC ev)
set_target_properties(EventBus PROPERTIES OUTPUT_NAME "EventBus")
set_target_properties(EventBus PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)