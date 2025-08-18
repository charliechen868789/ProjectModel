cmake_minimum_required(VERSION 3.12)

project(ConfigManager VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find RapidJSON
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(RapidJSON QUIET RapidJSON)
endif()

if(NOT RapidJSON_FOUND)
    find_path(RAPIDJSON_INCLUDE_DIRS
        NAMES rapidjson/rapidjson.h
        PATHS
            /usr/include
            /usr/local/include
            /opt/local/include
            ${CMAKE_INSTALL_PREFIX}/include
        DOC "RapidJSON include directory"
    )
    
    if(RAPIDJSON_INCLUDE_DIRS)
        set(RapidJSON_FOUND TRUE)
        set(RapidJSON_INCLUDE_DIRS ${RAPIDJSON_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "RapidJSON not found. Please install RapidJSON or set RAPIDJSON_INCLUDE_DIRS manually.")
    endif()
endif()

# Library source files
set(CONFIGMANAGER_SOURCES
    src/ConfigManager.cpp
)

set(CONFIGMANAGER_HEADERS
    include/ConfigManager.h
)

# Create the library
add_library(ConfigManager ${CONFIGMANAGER_SOURCES} ${CONFIGMANAGER_HEADERS})

target_include_directories(ConfigManager PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}
)