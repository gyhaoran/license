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
    hv_static
    OpenSSL::SSL OpenSSL::Crypto
)

add_subdirectory(python)

# Install rules
install(TARGETS ${PROJECT_NAME}
    DESTINATION lib
)

# install(FILES include/verify.h
#     DESTINATION include
# )
