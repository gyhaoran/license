cmake_minimum_required(VERSION 3.20)

project(license-manager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

set(JSON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rd/nlohmann)
set(LICENSE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/src/license)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/release/license-server")

add_subdirectory(src)
add_subdirectory(example)
