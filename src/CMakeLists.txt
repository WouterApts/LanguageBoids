set(HEADER_FILES
        Gregorian.h
        Julian.h
        Boid.h
        Config.h
        Simulation.h
        State.h
        StateManager.h
)

set(SOURCE_FILES
        Gregorian.cpp
        Julian.cpp
        Boid.cpp
        StateManager.cpp
        Application.cpp
        Application.h
        Simulation.cpp
        Camera.cpp
        Camera.h
        Obstacles.h
        Obstacles.cpp
        SpatialGrid.cpp
        SpatialGrid.h
        World.h
        Utility.cpp
        Utility.h
)

add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})

# SFML package configuration
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

# Link SFML libraries
# (PRIVATE: The dependency is only visible to the target being linked and not to any targets that depend on this target.)
# (PUBLIC: The dependency is only visible to the target being linked and not to any targets that depend on this target.)
target_link_libraries(${CMAKE_PROJECT_NAME}_lib PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)
