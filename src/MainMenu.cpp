//
// Created by wouter on 23-3-2024.
//

#include "MainMenu.h"

#include <iostream>
#include <SFML/Window/Event.hpp>
#include "Application.h"
#include "Simulation.h"
#include "editor/Editor.h"
#include "editor/Serialization.h"
#include "ui/Button.h"
#include "ui/Panel.h"

MainMenu::MainMenu(std::shared_ptr<Context>& context) : context(context) {

    auto& window = this->context->window;
    float start_menu_width = window->getSize().x/2;
    float start_menu_height = window->getSize().y/2;
    float pos_x = window->getSize().x/2 - start_menu_width/2;
    float pos_y = window->getSize().y/2 - start_menu_height/2;
    start_menu_interface = std::make_shared<Panel>(sf::Vector2f(pos_x, pos_y), start_menu_width, start_menu_height, sf::Color(100,100,100));

    std::string editor_btn_str = "EDITOR";
    std::shared_ptr<Interface> editor_btn = std::make_shared<TextButton>([&]() {StartEditor();}, editor_btn_str, sf::Color::White, 20, sf::Color::Transparent, sf::Vector2f(0,0), start_menu_width/5, start_menu_height/6);

    std::string simulation_btn_str = "LOAD SIMULATION";
    std::shared_ptr<Interface> simulator_btn = std::make_shared<TextButton>([&]() {StartSimulation();}, simulation_btn_str, sf::Color::White, 20, sf::Color::Transparent, sf::Vector2f(0,0), start_menu_width/3, start_menu_height/6);

    start_menu_interface->AddElementWithRelativePos(editor_btn, sf::Vector2f(start_menu_width*2/5, start_menu_height*1/4));
    start_menu_interface->AddElementWithRelativePos(simulator_btn, sf::Vector2f(start_menu_width*1/3, start_menu_height*2/4));
    start_menu_interface->active = true;

    interfaces.push_back(start_menu_interface);
}

void MainMenu::StartEditor() {
    World world = World{6000, 3000};
    auto editor = std::make_unique<Editor>(context, world, 1600, 900);
    context->state_manager->AddState(std::move(editor));
}

void MainMenu::StartSimulation() {
    std::string file_name = serialization::GetFileNameThroughLoadDialog();
    if (file_name.empty()) {
        std::cerr << "Error: Empty file name! " << std::endl;
        return;
    }

    auto loaded_world = serialization::LoadWorldFromFile(file_name);
    if (loaded_world) {
        World world = loaded_world.value();
        std::vector<float> language_statuses = {0.24, 0.26, 0.20, 0.30};
        auto simulation = std::make_unique<CompSimulation>(context, world, language_statuses, 1600, 900);
        context->state_manager->AddState(std::move(simulation), true);
    } else {
        std::cerr << "Error: Something wrong, cannot open file! " << std::endl;
        return;
    }
}

void MainMenu::Init() {
}

void MainMenu::ProcessInput() {

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    sf::Event event{};
    while(context->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            context->window->close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if interfaces are clicked.
                for (auto& interface : interfaces) {
                    if (interface->active) {
                        //Change view to default to acces UI coordinates. TODO: handle everything UI related in a InterfaceManager
                        auto mouse_interface_pos = context->window->mapPixelToCoords(mouse_pos, context->window->getDefaultView());
                        if (interface->IsPointInsideRect(mouse_interface_pos)) {
                            interface->OnClick(mouse_interface_pos);
                        }
                    }
                }
            }
        }


    }
}

void MainMenu::Update(sf::Time deltaTime) {}

void MainMenu::Pause() {}

void MainMenu::Draw() {
    context->window->clear(sf::Color::Black);
    for (auto& interface : interfaces) {
        if (interface->active) {
            interface->Draw(context->window.get());
        }
    }
    context->window->display();
}

void MainMenu::Start() {}
