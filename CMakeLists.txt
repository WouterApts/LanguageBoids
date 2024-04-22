cmake_minimum_required(VERSION 3.26)
project(Thesis)
set(CMAKE_CXX_STANDARD 20)
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
IF ( NOT DEFINED SOURCE_FILES)
    MESSAGE ( FATAL_ERROR "SOURCE_FILES is not defined and should be")
ENDIF (NOT DEFINED SOURCE_FILES)

# Prepend 'src/' to each file path
foreach(header ${HEADER_FILES})
    list(APPEND HEADER_FILES_FULL "src/${header}")
endforeach()
foreach(source ${SOURCE_FILES})
    list(APPEND SOURCE_FILES_FULL "src/${source}")
endforeach()

add_executable(Thesis_run main.cpp ${SOURCE_FILES_FULL} ${HEADER_FILES_FULL})

# SFML package configuration
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

# Link SFML libraries
# (PRIVATE: The dependency is only visible to the target being linked and not to any targets that depend on this target.)
# (PUBLIC: The dependency is visible to the target being linked and also to any targets that depend on this target.)
target_link_libraries(Thesis_run PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)

# Enable testing
# enable_testing()



