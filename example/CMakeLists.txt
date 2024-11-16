project(example)

include_directories(
    ${JSON_PATH}
    ${LICENSE_PATH}/include
)

add_executable(gen_license gen_license.cpp)
add_executable(gen_server_license gen_server_license.cpp)
add_executable(verify verify.cpp)

target_link_libraries(verify PRIVATE license)
target_link_libraries(gen_license PRIVATE license)
target_link_libraries(gen_server_license PRIVATE license)

install(TARGETS gen_server_license
    DESTINATION example
)

install(FILES private_key.pem
    DESTINATION example
)
