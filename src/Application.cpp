//
// Created by wouter on 19-2-2024.
//

#include <iostream>
#include "Application.h"
#include "World.h"
#include "Simulation.cpp"

Application::Application() : context(std::make_shared<Context>()){
    //Create application window
    context->window->create(sf::VideoMode(1600,900), "Language Boids");
}

void Application::Run() {

    // Create Simulation and push it to the State Stack, which will Initialize and Start it.
    World world{6000,3000};
    std::vector<float> language_statuses = {0.26, 0.24, 0.22, 0.28};
    std::unique_ptr<State> simulation = std::make_unique<CompSimulation>(context, world, language_statuses, 1600, 900);
    context->state_manager->AddState(std::move(simulation));

    while (context->window->isOpen()) {
        sf::Time deltaTime = clock.restart();

        context->state_manager->ProcessStateChange();
        context->state_manager->GetCurrentState()->ProcessInput();;
        context->state_manager->GetCurrentState()->Update(deltaTime);
        context->state_manager->GetCurrentState()->Draw();
    }
}

Application::~Application() = default;
