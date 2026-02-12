cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GDRconverter)
add_library(GDRconverter INTERFACE)

target_include_directories(GDRconverter INTERFACE include/)

if (NOT TARGET mat-msgpack)
    if (NOT COMMAND CPMAddPackage)
        include(cmake/CPM.cmake)
    endif()
    CPMAddPackage("gh:prevter/msgpack-for-matjson#ca2bb75")
endif()

if (NOT TARGET libGDR)
    include(FetchContent)
    FetchContent_Declare(
        libGDR
        GIT_REPOSITORY https://github.com/maxnut/GDReplayFormat.git
        GIT_TAG gdr2
        GIT_SHALLOW TRUE
        UPDATE_DISCONNECTED FALSE
    )
    FetchContent_MakeAvailable(libGDR)
endif()

target_link_libraries(GDRconverter INTERFACE mat-msgpack)

# Test
option(BUILD_TESTING "Build tests" OFF)
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR OR BUILD_TESTING)
    add_executable(GDRconverter_test test/main.cpp)
    target_link_libraries(GDRconverter_test PRIVATE GDRconverter libGDR)

    add_custom_command(
        TARGET GDRconverter_test POST_BUILD
        COMMAND GDRconverter_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
        COMMENT "Running test..."
    )
endif()