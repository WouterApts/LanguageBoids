//
// Created by wouter on 23-3-2024.
//

#ifndef MAINMENU_H
#define MAINMENU_H
#include "Application.h"
#include "SimulationData.h"
#include "State.h"
#include "ui/InterfaceManager.h"
#include "ui/components/Panel.h"


class MainMenu : public State {
public:

    std::shared_ptr<Context> context;
    std::shared_ptr<InterfaceManager> interface_manager;

    std::shared_ptr<Panel> start_menu_interface;

    explicit MainMenu(std::shared_ptr<Context> &context);

    void OpenEditorMenu();
    void StartSimulation();

    static std::string ExtractFileName(const std::string &filename);

    static KeySimulationData load_key_simulation_data(std::optional<SimulationData> loaded_data);
    static VectorSimulationData load_vector_simulation_data(std::optional<SimulationData> loaded_data);

    void Init() override;
    void ProcessInput() override;
    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;
};

class EditorMenu : public State {
public:

    std::shared_ptr<Context> context;
    std::shared_ptr<InterfaceManager> interface_manager;

    std::shared_ptr<Panel> editor_menu_interface;

    explicit EditorMenu(std::shared_ptr<Context> &context);

    void Init() override;
    void ProcessInput() override;
    void Update(sf::Time deltaTime) override;
    void Draw() override;
    void Pause() override;
    void Start() override;

    void StartEditorNew();
    void StartEditorFromFile();
};


#endif //MAINMENU_H
