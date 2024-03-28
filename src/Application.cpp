//
// Created by wouter on 19-2-2024.
//

#include <iostream>
#include "Application.h"

#include "MainMenu.h"
#include "ResourceManager.h"
#include "World.h"
#include "Simulator.cpp"
#include "Utility.h"
#include "editor/Editor.h"
#include "editor/Serialization.h"

Application::Application() : context(std::make_shared<Context>()){
    //Create application window
    context->window->create(sf::VideoMode(1600,900), "Language Boids");
    context->window->setFramerateLimit(static_cast<int>(FRAME_RATE));
}

void Application::Run() {

    InitializeRescources();

    // Create Simulation and push it to the State Stack, which will Initialize and Start it.
    World world{6000,3000};
    std::vector<float> language_statuses = {0.24, 0.26, 0.20, 0.30};

    //Simulation Test
    //std::unique_ptr<CompSimulation> simulation = std::make_unique<CompSimulation>(context, world, language_statuses, 1600, 900);
    //simulation->LoadWorldFromFile("test_world.dat");
    //context->state_manager->AddState(std::move(simulation));

    //Editor test
    //std::unique_ptr<State> editor = std::make_unique<Editor>(context, world, 1600, 900);
    //context->state_manager->AddState(std::move(editor));

    //Main menu
    std::unique_ptr<MainMenu> main_menu = std::make_unique<MainMenu>(context);
    context->state_manager->AddState(std::move(main_menu));

    while (context->window->isOpen()) {
        sf::Time delta_time = clock.restart();
        if (delta_time > TIME_PER_FRAME) delta_time = TIME_PER_FRAME;
        //PrintFPS(delta_time);

        context->state_manager->ProcessStateChange();
        context->state_manager->GetCurrentState()->ProcessInput();;
        context->state_manager->GetCurrentState()->Update(delta_time);
        context->state_manager->GetCurrentState()->Draw();
    }
}

Application::~Application() = default;

void Application::InitializeRescources() {
    // Images
    ResourceManager::LoadTexture("boid", "images/boid.png");
    ResourceManager::LoadTexture("boid_selection", "images/boid_selection.png");
    ResourceManager::LoadTexture("line_tool_button", "images/LineButton.png");
    ResourceManager::LoadTexture("circle_tool_button", "images/CircleButton.png");
    ResourceManager::LoadTexture("erase_tool_button", "images/EraseButton.png");
    ResourceManager::LoadTexture("boid_circle_button", "images/BoidCircleButton.png");
    ResourceManager::LoadTexture("boid_rectangle_button", "images/BoidRectangleButton.png");
    ResourceManager::LoadTexture("save_button", "images/SaveButton.png");
    ResourceManager::LoadTexture("terrain_button", "images/TerrainButton.png");
    ResourceManager::LoadTexture("config_button", "images/SettingsButton.png");
    ResourceManager::LoadTexture("grid_button", "images/GridButton.png");
    ResourceManager::LoadTexture("escape_button", "images/EscapeButton.png");

    // Fonts
    ResourceManager::LoadFont("arial", "fonts/arial.ttf");
}
