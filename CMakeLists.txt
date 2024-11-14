cmake_minimum_required(VERSION 3.20)

project(license-manager)

include(cmake/build.cmake)
include(cmake/dependency.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -w")

set(JSON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rd/nlohmann)
set(LICENSE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/src/license)
set(LICENSE_SERVER_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/src/license-server)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/release/license-server")

add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(test)

install(FILES config/env.sh
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

install(FILES config/license.dat
    DESTINATION ${CMAKE_INSTALL_PREFIX}/data
)
