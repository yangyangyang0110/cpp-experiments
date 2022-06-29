
add_executable(demo demo.cpp)
target_compile_options(
        demo
        PRIVATE
        -Werror -fPIC
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O2>
)

target_compile_definitions(
        demo
        PRIVATE
        $<$<CONFIG:Debug>:MyDebug>
)
