file(GLOB examples ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

dynamic_build_execute(cpp20 ${examples})

