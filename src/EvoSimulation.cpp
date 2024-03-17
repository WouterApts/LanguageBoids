//
// Created by wouter on 27-2-2024.
//


#include <random>
#include <iostream>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "Simulation.h"
#include "Utility.h"

EvoSimulation::EvoSimulation(std::shared_ptr<Context>& context, World& world, float camera_width, float camera_height)
    : Simulation(context, world, camera_width, camera_height),
      spatial_boid_grid(SpatialGrid<EvoBoid>(world.size().cast<int>(), static_cast<int>(INTERACTION_RADIUS/2)))
{}


void EvoSimulation::AddBoid(const std::shared_ptr<EvoBoid> &boid) {
    boids.push_back(boid);           // creates a copy of shared_ptr, assigning an additional owner (boids)
    spatial_boid_grid.AddObj(boid);
}


void EvoSimulation::Init() {
    //Create Random boids
    int boids_amount = 3000;
    for (int i = 0; i < boids_amount; ++i) {
        auto boid = std::make_shared<EvoBoid>(CreateRandomBoid(world.width, world.height, true));
        AddBoid(boid);
    }

    //Create Some Obstacles
    auto p1 = Eigen::Vector2f(1000,1000);
    auto p2 = Eigen::Vector2f(3000,1000);
    auto line = std::make_shared<LineObstacle>(p1, p2, 10, sf::Color::Yellow);
    world.obstacles.push_back(line);

    auto c1 = Eigen::Vector2f(1000, 1000);
    world.obstacles.push_back(std::make_shared<CircleObstacle>(c1, 150, sf::Color::White));
    auto c2 = Eigen::Vector2f(3000, 1500);
    world.obstacles.push_back(std::make_shared<CircleObstacle>(c2, 500, sf::Color::White));
};


void EvoSimulation::Update(sf::Time delta_time) {

    for (const auto& boid: boids) {

        //Get boids in perception radius:
        std::vector<EvoBoid*> perceived_boids = std::move(spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid));

        //Update boids acceleration
        boid->UpdateAcceleration(perceived_boids, world);

        //Update boids language
        boid->UpdateLanguage(perceived_boids, delta_time);
    }

    for (const auto& boid : boids) {
        //Update boids velocity (Also checking Collisions)
        boid->UpdateVelocity(world.obstacles, delta_time);

        //Update boids position
        boid->UpdatePosition(delta_time);
        spatial_boid_grid.UpdateObj(boid);

        //Update boids sprite
        boid->UpdateSprite();
    }
};

void EvoSimulation::ProcessInput() {

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    sf::Event event{};
    while(context->window->pollEvent(event)) {

        if (event.type == sf::Event::Closed) {
            context->window->close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            //Boid Selection
            ProcessBoidSelection(context.get(), mouse_pos, spatial_boid_grid);
            //Camera Drag
            camera.StartDragging(mouse_pos);
        }

        if (event.type == sf::Event::MouseButtonReleased) {
            //Camera Drag
            camera.StopDragging();
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            //Camera Zoom
            ProcessCameraZoom(event);
        }
    }

    if (IsKeyPressedOnce(sf::Keyboard::G)) {
        std::cout << "Visual Spatial Grid:" << !spatial_boid_grid.is_visible << std::endl;
        spatial_boid_grid.is_visible = !spatial_boid_grid.is_visible;
    }

    camera.Drag(mouse_pos);
};

void EvoSimulation::Draw() {
    context->window->clear(sf::Color::Black);
    context->window->setView(camera.view);

    // Draw Spatial Grid
    spatial_boid_grid.DrawGrid(context->window.get());

    // Draw Boids
    for (const auto& boid : boids) {
        context->window->draw(boid->sprite);
    }

    // Draw Obstacles
    for (const auto& obstacle : world.obstacles) {
        obstacle->Draw(context->window.get());
    }

    // Draw Boid Selection Circle
    DrawBoidSelectionCircle();

    DrawBorderOutline();
}


void EvoSimulation::Start() {

};


void EvoSimulation::Pause() {

};


EvoBoid EvoSimulation::CreateRandomBoid(float max_x_coord, float max_y_coord, bool random_language) {
    // Generate random position
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(0, static_cast<float>(max_x_coord));
    std::uniform_real_distribution<float> disY(0, static_cast<float>(max_y_coord));
    Eigen::Vector2f position(disX(gen), disY(gen));

    // Generate random velocity and acceleration
    std::uniform_real_distribution<float> disX2(MIN_SPEED, MAX_SPEED);
    std::uniform_real_distribution<float> disY2(MIN_SPEED, MAX_SPEED);
    Eigen::Vector2f velocity(disX2(gen), disY2(gen));
    Eigen::Vector2f acceleration(disX2(gen), disY2(gen));

    // Generate random language
    Eigen::VectorXi language = Eigen::VectorXi::Ones(LANGUAGE_SIZE);
    if (random_language) {
        std::uniform_int_distribution distrubution_language(0, 1);
        for (int i = 0; i < LANGUAGE_SIZE; ++i) {
            language(i) = distrubution_language(gen);
        }
    }

    float language_influence = 1.0f;

    // Create and return the Boid object
    return {position, velocity, acceleration, language, language_influence};
}

