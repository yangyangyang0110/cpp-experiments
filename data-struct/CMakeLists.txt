
add_executable(dag dag.cpp)
target_compile_options(
        dag
        PRIVATE
        -Werror -fPIC
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O2>
)

target_compile_definitions(
        dag
        PRIVATE
        $<$<CONFIG:Debug>:MyDebug>
)
