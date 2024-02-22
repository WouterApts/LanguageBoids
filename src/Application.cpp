//
// Created by wouter on 19-2-2024.
//

#include <iostream>
#include "Application.h"
#include "World.h"
#include "Simulation.h"


Application::Application() : context(std::make_shared<Context>()){
    //Create application window
    context->window->create(sf::VideoMode(1600,900), "Language Boids");
}

void Application::Run() {

    // Create Simulation and push it to the State Stack, which will Initialize and Start it.
    World world{1600,900};
    std::unique_ptr<State> simulation = std::make_unique<Simulation>(context, world, 100, 100);
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
