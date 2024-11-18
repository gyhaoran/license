project(license-server)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization)
find_package(spdlog REQUIRED)
find_package(libhv REQUIRED)


file(GLOB_RECURSE all_files
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${all_files})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${JSON_PATH}
    ${ARGPARSE_PATH}
    ${LICENSE_PATH}/include
)

target_link_libraries(${PROJECT_NAME} 
    license 
    pthread
    spdlog::spdlog
    hv_static 
    OpenSSL::SSL OpenSSL::Crypto 
    Boost::serialization
)

# Install rules       
install(TARGETS ${PROJECT_NAME}
    DESTINATION license-server/bin
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/license-server/lib"
)
