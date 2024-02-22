project(Google_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Google_Tests_run ConverterTest.cpp AbsoluteDateTest.cpp BoidTest.cpp)

target_link_libraries(Google_Tests_run ${CMAKE_PROJECT_NAME}_lib)
target_link_libraries(Google_Tests_run gtest gtest_main)