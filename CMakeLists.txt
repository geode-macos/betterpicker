cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CARGO_TARGET_DIR ${CMAKE_SOURCE_DIR}/rustlib/target)

project(betterpicker VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${CARGO_TARGET_DIR}/cxxbridge/rustlib/src/
)
target_sources(${PROJECT_NAME} PRIVATE ${CARGO_TARGET_DIR}/cxxbridge/rustlib/src/lib.rs.cc)
target_link_libraries(${PROJECT_NAME} ${CARGO_TARGET_DIR}/debug/rustlib.lib)
target_link_libraries(${PROJECT_NAME} kernel32 ntdll userenv ws2_32 dbghelp)

setup_geode_mod(${PROJECT_NAME})
