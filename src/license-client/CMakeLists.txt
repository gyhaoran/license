project(licensecli)

find_package(OpenSSL REQUIRED)
find_package(libhv REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${JSON_PATH}
)

file(GLOB_RECURSE all_files
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${all_files})
target_link_libraries(${PROJECT_NAME} 
    pthread
    hv
    OpenSSL::SSL OpenSSL::Crypto
)

add_subdirectory(python)

# Install rules
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION license-client/lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/license-client/lib"
)
