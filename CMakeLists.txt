cmake_minimum_required(VERSION 2.8)

file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/run_experiment.py DESTINATION ${CMAKE_BINARY_DIR})
enable_testing()
add_subdirectory(src)
add_subdirectory(test)
