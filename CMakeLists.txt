cmake_minimum_required(VERSION 3.10)

project(license)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/release")

find_package(OpenSSL REQUIRED)

# Find Python and pybind11
find_package(Python3 REQUIRED COMPONENTS Development Interpreter)
find_package(pybind11 REQUIRED)

# Print Python3 details for debugging
message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE all_files
    src/*.cpp
    python/bindings.cpp
)

add_library(${PROJECT_NAME} SHARED ${all_files})
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${Python3_INCLUDE_DIRS}
    ${pybind11_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto ${Python3_LIBRARIES})

add_subdirectory(example)

# Install rules
install(TARGETS ${PROJECT_NAME}
    DESTINATION lib
)

install(FILES include/verify.h
    DESTINATION include
)
