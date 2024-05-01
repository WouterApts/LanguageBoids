cmake_minimum_required(VERSION 3.26)
project(Thesis LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# Enable optimization flags for GCC and Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -static-libgcc")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Copy the images and fonts directory and its contents to the build directory
file(COPY images DESTINATION ${CMAKE_BINARY_DIR})
file(COPY fonts DESTINATION ${CMAKE_BINARY_DIR})

# Specify directories in which the compiler should search for header files.
# adds the specified directory to the include directories for all targets in the current CMakeLists.txt file and all its subdirectories.
# include_directories(C:/SFML/include)
include_directories(C:/Eigen-3.4.0)
include_directories(src)

# Add local subdirectories and read their CMakeList.txt files
add_subdirectory(src)

# Prepend 'src/' to each file path
foreach(header ${HEADER_FILES})
    list(APPEND HEADER_FILES_FULL "src/${header}")
endforeach()
foreach(source ${SOURCE_FILES})
    list(APPEND SOURCE_FILES_FULL "src/${source}")
endforeach()

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(Thesis_run main.cpp ${SOURCE_FILES_FULL} ${HEADER_FILES_FULL})

target_link_libraries(Thesis_run PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)
target_compile_features(Thesis_run PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
            TARGET Thesis_run
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Thesis_run>
            VERBATIM)
endif()

add_custom_command(
        TARGET Thesis_run POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/images $<TARGET_FILE_DIR:Thesis_run>/images
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:Thesis_run>/fonts
        COMMENT "Copying images and fonts directories" VERBATIM
)

add_custom_target(copy_resources ALL
        DEPENDS
        ${CMAKE_SOURCE_DIR}/images
        ${CMAKE_SOURCE_DIR}/fonts
)

add_dependencies(Thesis_run copy_resources)
install(TARGETS Thesis_run)

# SFML package configuration
# set(SFML_STATIC_LIBRARIES TRUE)
# set(SFML_DIR C:/SFML/lib/cmake/SFML)
#find_package(SFML COMPONENTS system window graphics audio network REQUIRED)
#
## Link SFML libraries
## (PRIVATE: The dependency is only visible to the target being linked and not to any targets that depend on this target.)
## (PUBLIC: The dependency is visible to the target being linked and also to any targets that depend on this target.)
#target_link_libraries(Thesis_run PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)



