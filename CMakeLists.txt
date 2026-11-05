cmake_minimum_required(VERSION 3.31.6)
project(MasterPasswordsCppCLI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc")

add_executable(MasterPasswordsCppCLI src/main.cc)

target_sources(MasterPasswordsCppCLI PRIVATE ${SOURCES} ${HEADERS})

# Позволяет писать #include "src/folder/file.h" из любой точки проекта
target_include_directories(MasterPasswordsCppCLI
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
