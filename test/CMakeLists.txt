file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
dynamic_build_execute("test" ${files})

