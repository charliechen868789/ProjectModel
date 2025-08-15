cmake_minimum_required(VERSION 3.16)

add_library(AppTemplate SHARED
    src/AppTemplate.cpp
)

target_include_directories(AppTemplate PUBLIC
    include
)

target_link_libraries(AppTemplate PUBLIC
    EventBus
)
