//
// Created by wouter on 20-2-2024.
//


#include <random>
#include <iostream>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "Simulation.h"
#include "Boid.h"
#include "Application.h"
#include "Camera.h"
#include "SpatialGrid.h"
#include "SpatialGrid.tpp"
#include "Utility.h"
#include "World.h"

Simulation::Simulation(std::shared_ptr<Context>& context, World& world, float camera_width, float camera_height)
    : context(context),
      world(world),
      camera(Camera(sf::Vector2f(world.width / 2, world.height / 2), camera_width, camera_height)),
      spatial_boid_grid(SpatialGrid<EvoBoid>(world.size().cast<int>(), static_cast<int>(PERCEPTION_RADIUS/2))) {

    selected_boid = nullptr;
};


void Simulation::AddBoid(const std::shared_ptr<EvoBoid> &boid) {
    boids.push_back(boid);           // creates a copy of shared_ptr, assigning an additional owner (boids)
    spatial_boid_grid.AddBoid(boid);
}

void Simulation::Init() {
    //Create Random boids
    int boids_amount = 3000;
    for (int i = 0; i < boids_amount; ++i) {
        auto boid = std::make_shared<EvoBoid>(CreateRandomEvoBoid(world.width, world.height, true));
        AddBoid(boid);
    }

    //Create Random Obstacles
    // auto p1 = Eigen::Vector2f(300,300);
    // auto p2 = Eigen::Vector2f(1200,1000);
    // auto line = std::make_shared<LineObstacle>(p1, p2, 10, sf::Color::Yellow);
    // obstacles.push_back(line);

    auto c1 = Eigen::Vector2f(1000, 1000);
    obstacles.push_back(std::make_shared<CircleObstacle>(c1, 150, sf::Color::White));
    auto c2 = Eigen::Vector2f(3000, 1500);
    obstacles.push_back(std::make_shared<CircleObstacle>(c2, 500, sf::Color::White));
};


void Simulation::Update(sf::Time delta_time) {

    for (const auto& boid: boids) {

        //Get boids in perception radius:
        std::vector<EvoBoid*> perceived_boids = std::move(spatial_boid_grid.RadiusSearch(boid->perception_radius, boid));

        //Update boids acceleration
        boid->UpdateAcceleration(perceived_boids, world);

        //Update boids language
        boid->UpdateLanguage(perceived_boids, delta_time);
    }

    for (const auto& boid : boids) {
        //Update boids velocity (Also checking Collisions)
        boid->UpdateVelocity(obstacles, delta_time);

        //Update boids position
        boid->UpdatePosition(delta_time);
        spatial_boid_grid.UpdateBoid(boid);

        //Update boids sprite
        boid->UpdateSprite();
    }
};


void Simulation::ProcessInput() {

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);

    sf::Event event;
    while(context->window->pollEvent(event)) {

        if (event.type == sf::Event::Closed) {
            context->window->close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            //Start camera Dragging
            auto sf_world_pos = context->window->mapPixelToCoords(mouse_pos, camera.view);
            std::cout << sf_world_pos.x << " " << sf_world_pos.y << std::endl;
            camera.StartDragging(mouse_pos);

            //Boid Selection
            auto world_pos = Eigen::Vector2f(sf_world_pos.x, sf_world_pos.y);
            auto selected_boids = spatial_boid_grid.LocalSearch(world_pos);
            for (auto& boid : selected_boids) {
                if ((boid->pos - world_pos).norm() <= boid->collision_radius) {
                    if (boid == selected_boid) selected_boid = nullptr;
                    else selected_boid = boid;
                    break;
                }
            }
        }

        if (event.type == sf::Event::MouseButtonReleased) {
            //Stop camera Dragging
            camera.StopDragging();
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            //Zoom in and out
            if (event.mouseWheelScroll.delta > 0) {
                camera.Zoom(-0.1); // Zoom in
            } else if (event.mouseWheelScroll.delta < 0) {
                camera.Zoom(0.1); // Zoom out
            }
        }

    }

    if (isKeyPressedOnce(sf::Keyboard::G)) {
        std::cout << "Key G is pressed once!" << std::endl;
        spatial_boid_grid.is_visible = !spatial_boid_grid.is_visible;
    }

    camera.Drag(mouse_pos);
};


void Simulation::Draw() {
    context->window->clear(sf::Color::Black);
    context->window->setView(camera.view);

    // Draw Spatial Grid
    spatial_boid_grid.DrawGrid(context->window.get());

    // Draw Boids
    for (const auto& boid : boids) {
        context->window->draw(boid->sprite);
    }

    // Draw Obstacles
    for (const auto& obstacle : obstacles) {
        obstacle->Draw(context->window.get());
    }

    // Draw Boid Selection Circle
    if (selected_boid != nullptr) {
        float r = 20;
        auto selection_circle = sf::CircleShape(20);
        selection_circle.setPosition(selected_boid->pos.x(), selected_boid->pos.y());
        selection_circle.setOrigin(r, r);
        selection_circle.setFillColor(sf::Color::Transparent);
        selection_circle.setOutlineThickness(5);
        context->window->draw(selection_circle);
    }

    auto border_rect = sf::RectangleShape(sf::Vector2f(world.width, world.height));
    border_rect.setFillColor(sf::Color::Transparent);
    border_rect.setOutlineColor(sf::Color::White);
    border_rect.setOutlineThickness(10);
    context->window->draw(border_rect);
    context->window->display();
}


void Simulation::Start() {

};


void Simulation::Pause() {

};


EvoBoid CreateRandomEvoBoid(const int max_x_coord, const int max_y_coord, bool random_language) {
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

sf::Vector2f EigenVectorToSFML(const Eigen::Vector2f& eigenVec) {
    return {eigenVec.x(), eigenVec.y()};
}

struct BoidParam {
    Eigen::Vector2f position;
    Eigen::Vector2f velocity;
    Eigen::Vector2f acceleration;
    Eigen::VectorXi language;
    float language_influence;
};