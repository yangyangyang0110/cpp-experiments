
add_executable(test test.cpp)
target_compile_options(
        test
        PRIVATE
        -Werror -fPIC
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O2>
)


target_compile_definitions(
        test
        PRIVATE
        $<$<CONFIG:Debug>:MyDebug>
)

target_link_libraries(
        test
        PRIVATE
        pthread
)

