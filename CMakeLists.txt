cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(experiments VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
)

#find_package(spdlog REQUIRED)

#include(${CMAKE_CURRENT_SOURCE_DIR}/build/conanbuildinfo.cmake)
#conan_basic_setup()

function(dynamic_build_execute prefix)
    foreach (example ${ARGN})
        get_filename_component(target ${example} NAME_WLE)
        string(REPLACE "_" "-" target ${target})
        if (NOT ${prefix} STREQUAL "")
            set(target "${prefix}-${target}")
        endif ()
        message(STATUS "build ${example} to ${target}...")
        add_executable(${target} ${example})

        target_compile_options(
                ${target}
                PRIVATE
                -Werror -fPIC
                $<$<CONFIG:Debug>:-g -O0>
                $<$<CONFIG:Release>:-O2>
                #                -ftemplate-depth=10000000
        )

        target_compile_definitions(
                ${target}
                PRIVATE
                $<$<CONFIG:Debug>:MyDebug>
        )

        target_link_libraries(
                ${target}
                PRIVATE
                pthread
        )
    endforeach ()
endfunction()


add_subdirectory(logger)
add_subdirectory(sfinae)
add_subdirectory(demo)
add_subdirectory(data-struct)
add_subdirectory(meta-programming)
add_subdirectory(cpp20)
add_subdirectory(concurrency)
add_subdirectory(test)
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()




