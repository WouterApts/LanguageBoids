cmake_minimum_required(VERSION 3.26)
project(Thesis)
set(CMAKE_CXX_STANDARD 20)
set(SOURCE_FILES main.cpp)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable optimization flags for GCC and Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

add_executable(Thesis_run ${SOURCE_FILES})

# Copy the images directory and its contents to the build directory
file(COPY images DESTINATION ${CMAKE_BINARY_DIR})

# Include_directories specifies directories in which the compiler should search for header files.
# This command affects the build properties of targets within the current CMakeLists.txt file or the targets it includes.
include_directories(src)
include_directories(C:/SFML/include)
include_directories(C:/Eigen-3.4.0)

add_subdirectory(src)
add_subdirectory(Google_tests)

# ${CMAKE_PROJECT_NAME}_lib comes with the SFML dependency
target_link_libraries(Thesis_run ${CMAKE_PROJECT_NAME}_lib)

# Enable testing
enable_testing()



