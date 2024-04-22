set(HEADER_FILES
        Boid.h
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
        Terrain.h
        ResourceManager.h
        TimedEvent.h
        DominanceStudySimulator.h
        BoidSpawners.h
        MainMenu.h
        SimulationData.h
        SimulationConfig.h

        editor/Editor.h
        editor/Tools.h
        editor/ToolSelector.h
        editor/Serialization.h

        analysis/KeyAnalyser.h

        ui/components/Button.h
        ui/components/Panel.h
        ui/components/InputField.h
        ui/ToolSelectorInterface.h
        ui/TerrainToolInterface.h
        ui/InterfaceManager.h
        ui/BoidToolInterfaces.h
        ui/SettingsInterface.h
        ui/components/InterfaceComponent.h
        ui/EscapeStateInterface.h
        ui/ConfigInterface.h
        ui/WorldSizeInterface.h
        ui/StudyInterface.h
        ui/components/TextField.h

        PARENT_SCOPE
)

set(SOURCE_FILES
        Boid.cpp
        StateManager.cpp
        Application.cpp
        Simulator.cpp
        Camera.cpp
        Obstacles.cpp
        Utility.cpp
        # VectorSimulator.cpp
        KeySimulator.cpp
        KeyBoid.cpp
        VectorBoid.cpp
        LanguageManager.cpp
        Terrain.cpp
        ResourceManager.cpp
        BoidSpawners.cpp
        MainMenu.cpp
        DominanceStudySimulator.cpp

        analysis/KeyAnalyser.cpp

        editor/Editor.cpp
        editor/Tools.cpp
        editor/ToolSelector.cpp
        editor/Serialization.cpp

        ui/components/Button.cpp
        ui/components/Panel.cpp
        ui/components/InputField.cpp
        ui/components/InterfaceComponent.cpp
        ui/ToolSelectorInterface.cpp
        ui/TerrainToolInterface.cpp
        ui/InterfaceManager.cpp
        ui/components/TextField.cpp
        ui/BoidToolInterfaces.cpp
        ui/EscapeStateInterface.cpp
        ui/SettingsInterface.cpp
        ui/ConfigInterface.cpp
        ui/WorldSizeInterface.cpp
        ui/StudyInterface.cpp

        PARENT_SCOPE
)

# add_library(${CMAKE_PROJECT_NAME}_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
