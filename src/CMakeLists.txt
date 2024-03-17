set(HEADER_FILES
        Boid.h
        Config.h
        Simulation.h
        State.h
        StateManager.h
        Camera.h
        Obstacles.h
        Application.h
        LanguageManager.h
        Utility.h
        SpatialGrid.h
        SpatialGrid.tpp
        World.h
)

set(SOURCE_FILES
        Boid.cpp
        StateManager.cpp
        Application.cpp
        Simulation.cpp
        Camera.cpp
        Obstacles.cpp
        Utility.cpp
        EvoSimulation.cpp
        CompSimulation.cpp
        CompBoid.cpp
        EvoBoid.cpp
        LanguageManager.cpp
        Terrain.cpp
        Terrain.h
        Analysis/CompAnalyser.cpp
        Analysis/CompAnalyser.h
        editor/Editor.cpp
        editor/Editor.h
        editor/SpawnZone.cpp
        editor/SpawnZone.h
        ui/Interface.cpp
        ui/Interface.h
        RescourceManager.cpp
        RescourceManager.h
        editor/Tools.cpp
        editor/Tools.h
        ui/Button.cpp
        ui/Button.h
        ui/Panel.cpp
        ui/Panel.h
)

add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})

# SFML package configuration
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

# Link SFML libraries
# (PRIVATE: The dependency is only visible to the target being linked and not to any targets that depend on this target.)
# (PUBLIC: The dependency is visible to the target being linked and also to any targets that depend on this target.)
target_link_libraries(${CMAKE_PROJECT_NAME}_lib PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)