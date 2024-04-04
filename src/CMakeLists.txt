set(HEADER_FILES
        Boid.h
        Config.h
        Simulator.h
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
        Simulator.cpp
        Camera.cpp
        Obstacles.cpp
        Utility.cpp
        #VectorSimulator.cpp
        KeySimulator.cpp
        KeyBoid.cpp
        VectorBoid.cpp
        LanguageManager.cpp
        Terrain.cpp
        Terrain.h
        Analysis/CompAnalyser.cpp
        Analysis/CompAnalyser.h
        editor/Editor.cpp
        editor/Editor.h
        ResourceManager.cpp
        ResourceManager.h
        editor/Tools.cpp
        editor/Tools.h
        ui/components/Button.cpp
        ui/components/Button.h
        ui/components/Panel.cpp
        ui/components/Panel.h
        ui/components/InputField.cpp
        ui/components/InputField.h
        editor/ToolSelector.cpp
        editor/ToolSelector.h
        ui/ToolSelectorInterface.cpp
        ui/ToolSelectorInterface.h
        ui/TerrainToolInterface.cpp
        ui/TerrainToolInterface.h
        TimedEvent.h
        ui/InterfaceManager.cpp
        ui/InterfaceManager.h
        ui/components/TextField.cpp
        ui/components/TextField.h
        ui/BoidToolInterfaces.cpp
        ui/BoidToolInterfaces.h
        BoidSpawners.cpp
        BoidSpawners.h
        editor/Serialization.cpp
        editor/Serialization.h
        ui/SettingsInterface.cpp
        ui/SettingsInterface.h
        MainMenu.cpp
        MainMenu.h
        ui/components/InterfaceComponent.cpp
        ui/components/InterfaceComponent.h
        ui/EscapeInterface.cpp
        ui/EscapeInterface.h
        SimulationData.h
        SimulationConfig.h
        ui/ConfigInterface.cpp
        ui/ConfigInterface.h
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