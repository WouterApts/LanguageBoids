//
// Created by wouter on 23-3-2024.
//

#ifndef MAINMENU_H
#define MAINMENU_H
#include "Application.h"
#include "State.h"
#include "ui/Interface.h"
#include "ui/Panel.h"


class MainMenu : public State {
public:

    std::shared_ptr<Context> context;
    std::vector<std::shared_ptr<Interface>> interfaces;

    std::shared_ptr<Panel> start_menu_interface;
    std::shared_ptr<Panel> editor_menu_interface;

    explicit MainMenu(std::shared_ptr<Context> &context);

    void StartEditor();
    void StartSimulation();

    void Init() override;
    void ProcessInput() override;
    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;
};



#endif //MAINMENU_H
