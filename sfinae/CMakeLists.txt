
add_executable(main main.cpp)
target_compile_options(
        main
        PRIVATE
        -Werror -fPIC
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O2>
)

target_compile_definitions(
        main
        PRIVATE
        $<$<CONFIG:Debug>:MyDebug>
)
