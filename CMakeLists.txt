cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(cmake/CPM.cmake)

project(GDRconverter)

add_library(GDRconverter INTERFACE)

target_include_directories(GDRconverter INTERFACE include/)

# Test

add_executable(GDRconverter_test
    test/main.cpp
)

CPMAddPackage("gh:maxnut/GDReplayFormat#gdr2")

target_link_libraries(GDRconverter_test PRIVATE GDRconverter libGDR)

add_custom_command(
    TARGET GDRconverter_test POST_BUILD
    COMMAND GDRconverter_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    COMMENT "Running test..."
)