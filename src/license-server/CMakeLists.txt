project(license-server)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization)
find_package(spdlog REQUIRED)
find_package(libhv REQUIRED)

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

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

function(install_library_targets targets destination)
    foreach(target_name ${targets})
        install(FILES $<TARGET_FILE:${target_name}> 
                DESTINATION ${destination})
    endforeach()
endfunction()

install_library_targets("Boost::serialization" license-server/lib)

install_real_files_from_targets(
    "OpenSSL::SSL;OpenSSL::Crypto"
    license-server/lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN/../lib64"
)
