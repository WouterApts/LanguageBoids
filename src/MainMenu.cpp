//
// Created by wouter on 23-3-2024.
//

#include "MainMenu.h"

#include <iostream>
#include <SFML/Window/Event.hpp>
#include "Application.h"
#include "CompStudySimulator.h"
#include "Simulator.h"
#include "Utility.h"
#include "editor/Editor.h"
#include "Serialization.h"
#include "ui/components/Button.h"
#include "ui/components/Panel.h"

MainMenu::MainMenu(std::shared_ptr<Context>& context) : context(context) {}

void MainMenu::OpenEditorMenu() {
    auto editor_menu = std::make_unique<EditorMenu>(context);
    context->state_manager->AddState(std::move(editor_menu));
}

void MainMenu::StartSimulation() {

    std::string file_name = serialization::GetFileNameThroughLoadDialog();

    if (file_name.empty()) {
        std::cerr << "Error: Empty file name! " << std::endl;
        return;
    }

    if (auto loaded_data = serialization::LoadSimulationDataFromFile(file_name)) {
        auto simulation_name = ExtractFileName(file_name);
        if (loaded_data->type == CompSimulation) {
            KeySimulationData simulation_data = load_key_simulation_data(loaded_data);
            auto simulation = std::make_unique<CompSimulator>(context, simulation_data, simulation_name, 1600, 900);
            context->state_manager->AddState(std::move(simulation));
        }else if (loaded_data->type == EvoSimulation) {
            VectorSimulationData simulation_data = load_vector_simulation_data(loaded_data);
            auto simulation_file_name = ExtractFileName(file_name);
            auto simulation = std::make_unique<EvoSimulator>(context, simulation_data, simulation_name, 1600, 900);
            context->state_manager->AddState(std::move(simulation));
        }
        else if (loaded_data->type == CompStudy) {
            KeySimulationData simulation_data = load_key_simulation_data(loaded_data);
            auto simulation = std::make_unique<CompStudySimulator>(context, simulation_data, simulation_name, 1600, 900);
            //simulation->fast_analysis = true; //Skip testing distributions once a completely dominant one has been found.
            context->state_manager->AddState(std::move(simulation));
        }

    } else {
        std::cerr << "Error: Something wrong, cannot open file! " << std::endl;
        return;
    }
}

std::string MainMenu::ExtractFileName(const std::string& filename) {
    size_t lastSlashPos = filename.find_last_of('\\');
    size_t lastDotPos = filename.find_last_of('.');
    if (lastSlashPos != std::string::npos && lastDotPos != std::string::npos) {
        return filename.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
    }
    return filename; // No extension found
}

void MainMenu::Init() {
    interface_manager = std::make_shared<InterfaceManager>();
    auto& window = this->context->window;
    float start_menu_width = window->getSize().x/3;
    float start_menu_height = window->getSize().y/3;
    float pos_x = window->getSize().x/2 - start_menu_width/2;
    float pos_y = window->getSize().y/2 - start_menu_height/2;
    start_menu_interface = std::make_shared<Panel>(sf::Vector2f(pos_x, pos_y), sf::Vector2f(start_menu_width, start_menu_height));

    std::shared_ptr<TextField> title_text_fld = std::make_shared<TextField>(sf::Vector2f(0,0), 70, "LANGUAGE BOIDS", *ResourceManager::GetFont("oswald"), sf::Color::White);
    title_text_fld->setPosition(window->getSize().x/2 - title_text_fld->text.getGlobalBounds().width/2, pos_y - 150);

    std::string editor_btn_str = "EDITOR";
    auto editor_btn = std::make_shared<TextButton>([&]() {OpenEditorMenu();}, editor_btn_str, sf::Vector2f(0,0), sf::Vector2f(start_menu_width/2, start_menu_height/6), 20.f);
    editor_btn->Activate();

    std::string simulation_btn_str = "LOAD SIMULATION";
    auto simulator_btn = std::make_shared<TextButton>([&]() {StartSimulation();}, simulation_btn_str,
                                                      sf::Vector2f(0,0), sf::Vector2f(start_menu_width/2, start_menu_height/6), 20.f);

    start_menu_interface->AddComponentWithRelativePos(editor_btn, sf::Vector2f(start_menu_width*1/4, start_menu_height*1/4));
    start_menu_interface->AddComponentWithRelativePos(simulator_btn, sf::Vector2f(start_menu_width*1/4, start_menu_height*2/4));
    start_menu_interface->Activate();

    interface_manager->AddComponent(start_menu_interface);
    interface_manager->AddComponent(title_text_fld);
}

void MainMenu::ProcessInput() {

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    sf::Event event{};
    while(context->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            context->window->close();
        }

        auto mouse_interface_pos = context->window->mapPixelToCoords(mouse_pos, context->window->getDefaultView());
        interface_manager->OnMouseEnter(mouse_interface_pos);
        interface_manager->OnMouseLeave(mouse_interface_pos);

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if interfaces are clicked.
                interface_manager->OnLeftClick(mouse_interface_pos);
            }
        }
    }
}

void MainMenu::Update(sf::Time deltaTime) {}

void MainMenu::Pause() {}

void MainMenu::Draw() {
    context->window->clear(sf::Color::Black);

    interface_manager->DrawComponents(context->window.get());

    context->window->display();
}

void MainMenu::Start() {
    context->window->setView(context->window->getDefaultView());
}


KeySimulationData MainMenu::load_key_simulation_data(std::optional<SimulationData> loaded_data) {
    auto simulation_data = KeySimulationData(loaded_data->type, loaded_data->world, loaded_data->config);
    // Cast each BoidSpawner pointer to KeyBoidSpawner pointer
    std::transform(loaded_data->boid_spawners.begin(), loaded_data->boid_spawners.end(), std::back_inserter(simulation_data.boid_spawners),
                   [](const std::shared_ptr<BoidSpawner>& spawner) {
                       return std::dynamic_pointer_cast<CompBoidSpawner>(spawner);
                   });
    return simulation_data;
}

VectorSimulationData MainMenu::load_vector_simulation_data(std::optional<SimulationData> loaded_data) {
    auto simulation_data = VectorSimulationData(loaded_data->type, loaded_data->world, loaded_data->config);
    // Cast each BoidSpawner pointer to KeyBoidSpawner pointer
    std::transform(loaded_data->boid_spawners.begin(), loaded_data->boid_spawners.end(), std::back_inserter(simulation_data.boid_spawners),
                   [](const std::shared_ptr<BoidSpawner>& spawner) {
                       return std::dynamic_pointer_cast<EvoBoidSpawner>(spawner);
                   });
    return simulation_data;
}


EditorMenu::EditorMenu(std::shared_ptr<Context>& context) : context(context) {}

void EditorMenu::Init() {
    interface_manager = std::make_shared<InterfaceManager>();
    auto& window = this->context->window;
    float start_menu_width = window->getSize().x/3;
    float start_menu_height = window->getSize().y/3;
    float pos_x = window->getSize().x/2 - start_menu_width/2;
    float pos_y = window->getSize().y/2 - start_menu_height/2;
    editor_menu_interface = std::make_shared<Panel>(sf::Vector2f(pos_x, pos_y), sf::Vector2f(start_menu_width, start_menu_height));

    std::shared_ptr<TextField> title_text_fld = std::make_shared<TextField>(sf::Vector2f(0,0), 70, "LANGUAGE BOIDS", *ResourceManager::GetFont("oswald"), sf::Color::White);
    title_text_fld->setPosition(window->getSize().x/2 - title_text_fld->text.getGlobalBounds().width/2, pos_y - 150);
    std::shared_ptr<TextField> editor_text_fld = std::make_shared<TextField>(sf::Vector2f(0,0), 35, " - EDITOR -", *ResourceManager::GetFont("oswald"), sf::Color::White);
    editor_text_fld->setPosition(window->getSize().x/2 - editor_text_fld->text.getGlobalBounds().width/2, pos_y - 75);

    std::string editor_new_txt = "CREATE NEW";
    std::shared_ptr<InterfaceComponent> editor_new_btn = std::make_shared<TextButton>([&]() {StartEditorNew();}, editor_new_txt, sf::Vector2f(0,0), sf::Vector2f(start_menu_width/2, start_menu_height/6), 20.f);
    editor_new_btn->Activate();

    std::string editor_from_file_txt = "EDIT EXISTING";
    std::shared_ptr<InterfaceComponent> editor_from_file_btn = std::make_shared<TextButton>([&]() {StartEditorFromFile();}, editor_from_file_txt,
                                                                                     sf::Vector2f(0,0), sf::Vector2f(start_menu_width/2, start_menu_height/6), 20.f);

    editor_menu_interface->AddComponentWithRelativePos(editor_new_btn, sf::Vector2f(start_menu_width*1/4, start_menu_height*1/4));
    editor_menu_interface->AddComponentWithRelativePos(editor_from_file_btn, sf::Vector2f(start_menu_width*1/4, start_menu_height*2/4));
    editor_menu_interface->Activate();

    interface_manager->AddComponent(editor_menu_interface);
    interface_manager->AddComponent(title_text_fld);
    interface_manager->AddComponent(editor_text_fld);
}

void EditorMenu::ProcessInput() {

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    sf::Event event{};
    while(context->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            context->window->close();
        }

        auto mouse_interface_pos = context->window->mapPixelToCoords(mouse_pos, context->window->getDefaultView());
        interface_manager->OnMouseEnter(mouse_interface_pos);
        interface_manager->OnMouseLeave(mouse_interface_pos);

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if interfaces are clicked.
                interface_manager->OnLeftClick(mouse_interface_pos);
            }
        }

        if (IsKeyPressedOnce(sf::Keyboard::Escape)) {
            context->state_manager->PopState();
        }
    }
}

void EditorMenu::Update(sf::Time deltaTime) {
}

void EditorMenu::Draw() {
    context->window->clear(sf::Color::Black);

    interface_manager->DrawComponents(context->window.get());

    context->window->display();
}

void EditorMenu::Pause() {
}

void EditorMenu::Start() {
}

void EditorMenu::StartEditorNew() {
    auto world = World{4000, 2000};
    auto configuration = std::make_shared<SimulationConfig>();
    auto simulation_data = SimulationData(CompSimulation, world, configuration);
    auto editor = std::make_unique<Editor>(context, simulation_data, 1600, 900);
    context->state_manager->AddState(std::move(editor), true);
}

void EditorMenu::StartEditorFromFile() {
    std::string file_name = serialization::GetFileNameThroughLoadDialog();

    if (file_name.empty()) {
        std::cerr << "Error: Empty file name! " << std::endl;
        return;
    }

    if (auto loaded_data = serialization::LoadSimulationDataFromFile(file_name)) {
        SimulationData simulation_data = MainMenu::load_key_simulation_data(loaded_data);
        auto editor = std::make_unique<Editor>(context, simulation_data, 1600, 900);
        context->state_manager->AddState(std::move(editor), true);

    } else {
        std::cerr << "Error: Something wrong, cannot open file! " << std::endl;
        return;
    }
}

