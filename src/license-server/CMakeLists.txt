project(license-server)

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE all_files
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${all_files})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${JSON_PATH}
    ${LICENSE_PATH}/include
)

target_link_libraries(${PROJECT_NAME} license hv_static OpenSSL::SSL OpenSSL::Crypto)

# Install rules
install(TARGETS ${PROJECT_NAME}
    DESTINATION bin
)

install(FILES include/verify.h
    DESTINATION include
)
