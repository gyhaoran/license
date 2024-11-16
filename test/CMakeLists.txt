project(license-ft)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization)
find_package(spdlog REQUIRED)
find_package(libhv REQUIRED)
find_package(GTest REQUIRED)

file(GLOB_RECURSE test_files
    ${LICENSE_SERVER_PATH}/src/app/*.cpp
    ${LICENSE_SERVER_PATH}/src/domain/*.cpp
    ${LICENSE_SERVER_PATH}/src/infra/*.cpp
    ${LICENSE_SERVER_PATH}/src/service/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${test_files} "gtest_main.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${JSON_PATH}
    ${ARGPARSE_PATH}
    ${LICENSE_PATH}/include
    ${LICENSE_SERVER_PATH}/include
)

target_link_libraries(${PROJECT_NAME} license GTest::gtest spdlog::spdlog pthread hv_static OpenSSL::SSL OpenSSL::Crypto Boost::serialization)

# Install rules       
install(TARGETS ${PROJECT_NAME}
    DESTINATION bin
)
