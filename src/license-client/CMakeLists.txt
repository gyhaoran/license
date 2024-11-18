project(licensecli)

find_package(OpenSSL REQUIRED)
find_package(libhv REQUIRED)
find_package(TBB REQUIRED)

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

add_executable(press_tool ${CMAKE_CURRENT_SOURCE_DIR}/press_tool.cpp)
target_link_libraries(press_tool  PRIVATE TBB::tbb ${PROJECT_NAME})

add_subdirectory(python)

# Install rules
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION license-client/lib
)

install(FILES ${LIBHV_PATH}/lib/libhv.so
        DESTINATION license-client/lib
)
