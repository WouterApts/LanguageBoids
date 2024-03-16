//
// Created by wouter on 19-2-2024.
//

#include <iostream>
#include "Application.h"

#include "RescourceManager.h"
#include "World.h"
#include "Simulation.cpp"
#include "Utility.h"

Application::Application() : context(std::make_shared<Context>()){
    //Create application window
    context->window->create(sf::VideoMode(1600,900), "Language Boids");
    context->window->setFramerateLimit(FRAME_RATE);
}

void Application::Run() {
    InitializeRescources();

    // Create Simulation and push it to the State Stack, which will Initialize and Start it.
    World world{6000,3000};
    std::vector<float> language_statuses = {0.24, 0.26, 0.20, 0.30};
    std::unique_ptr<State> simulation = std::make_unique<CompSimulation>(context, world, language_statuses, 1600, 900);
    context->state_manager->AddState(std::move(simulation));

    while (context->window->isOpen()) {
        sf::Time delta_time = clock.restart();
        if (delta_time > TIME_PER_FRAME) delta_time = TIME_PER_FRAME;
        PrintFPS(delta_time);

        context->state_manager->ProcessStateChange();
        context->state_manager->GetCurrentState()->ProcessInput();;
        context->state_manager->GetCurrentState()->Update(delta_time);
        context->state_manager->GetCurrentState()->Draw();
    }
}

Application::~Application() = default;

void Application::InitializeRescources() {
    RescourceManager::LoadTexture("boid", "images/boid.png");
    RescourceManager::LoadTexture("boid_selection", "images/boid_selection.png");
}
