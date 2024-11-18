project(licensepy)

# Find Python and pybind11
find_package(Python3 REQUIRED COMPONENTS Development Interpreter)
find_package(pybind11 REQUIRED)

# Print Python3 details for debugging
message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
message(STATUS "Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3_LIBRARIES: ${Python3_LIBRARIES}")


add_library(${PROJECT_NAME} SHARED bindings.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${Python3_INCLUDE_DIRS}
    ${pybind11_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} licensecli ${Python3_LIBRARIES})

# Install rules
install(TARGETS ${PROJECT_NAME}
    DESTINATION license-client/lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

function(install_with_symlink_and_target lib dest_dir)
    execute_process(COMMAND readlink -f ${lib} OUTPUT_VARIABLE real_file OUTPUT_STRIP_TRAILING_WHITESPACE)
    install(FILES ${lib} DESTINATION ${dest_dir} 
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_EXECUTE
                        WORLD_READ WORLD_EXECUTE)

    install(FILES ${real_file} DESTINATION ${dest_dir}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                        GROUP_READ GROUP_EXECUTE
                        WORLD_READ WORLD_EXECUTE)
endfunction()

foreach(lib ${Python3_LIBRARIES})
    install_with_symlink_and_target(${lib} license-client/lib)
endforeach()
