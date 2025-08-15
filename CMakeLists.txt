cmake_minimum_required(VERSION 3.10)
project(EventDrivenSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Common)
add_subdirectory(VirtualSensor)
add_subdirectory(Algorithm)
add_subdirectory(WebApp)
add_subdirectory(Gui)