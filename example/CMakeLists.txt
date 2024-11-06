project(example)

add_executable(gen_license gen_license.cpp)
add_executable(verify verify.cpp)

target_link_libraries(verify PRIVATE license)
target_link_libraries(gen_license PRIVATE license)
