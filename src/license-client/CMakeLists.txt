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

function(install_real_files_from_targets targets dest_dir)
    foreach(target ${targets})
        if (TARGET ${target})
            get_target_property(target_path ${target} IMPORTED_LOCATION)

            if (NOT target_path)
                message(FATAL_ERROR "Failed to get the IMPORTED_LOCATION of ${target}")
            endif()

            execute_process(COMMAND readlink -f ${target_path} OUTPUT_VARIABLE real_file OUTPUT_STRIP_TRAILING_WHITESPACE)

            install(FILES ${target_path} DESTINATION ${dest_dir})

            if (NOT "${target_path}" STREQUAL "${real_file}")
                install(FILES ${real_file} DESTINATION ${dest_dir})
            endif()
        else()
            message(FATAL_ERROR "Target ${target} not found!")
        endif()
    endforeach()
endfunction()

install_real_files_from_targets(
    "OpenSSL::SSL;OpenSSL::Crypto"
    license-client/lib
)
