file(GLOB TEMPLATE_SRC src/*.cpp)
add_library(AppTemplate STATIC ${TEMPLATE_SRC})
target_include_directories(AppTemplate PUBLIC include ../EventBus/include)
target_link_libraries(AppTemplate PUBLIC EventBus)
