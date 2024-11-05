cmake_minimum_required(VERSION 3.10)

project(license)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/release")

find_package(OpenSSL REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE all_files
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${all_files})
add_executable(main main.cpp)
add_executable(verify verify.cpp)

target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(verify ${PROJECT_NAME})
target_link_libraries(main ${PROJECT_NAME})
