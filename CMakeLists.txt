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

# Copy the images and fonts directory and its contents to the build directory
file(COPY images DESTINATION ${CMAKE_BINARY_DIR})
file(COPY fonts DESTINATION ${CMAKE_BINARY_DIR})

# Specify directories in which the compiler should search for header files.
# adds the specified directory to the include directories for all targets in the current CMakeLists.txt file and all its subdirectories.
include_directories(C:/SFML/include)
include_directories(C:/Eigen-3.4.0)

include_directories(src)

# Add local subdirectories and read their CMakeList.txt files
add_subdirectory(src)
add_subdirectory(Google_tests)

# Create build executable
add_executable(Thesis_run ${SOURCE_FILES})
target_link_libraries(Thesis_run ${CMAKE_PROJECT_NAME}_lib) # adding Thesis_lib to build (SFML already linked)

# Enable testing
# enable_testing()



