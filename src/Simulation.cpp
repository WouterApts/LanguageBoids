//
// Created by wouter on 20-2-2024.
//


#include <random>
#include <iostream>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include "Boid.h"
#include "Simulation.h"

#include "Application.h"
#include "Camera.h"
#include "SpatialGrid.h"
#include "World.h"

Simulation::Simulation(std::shared_ptr<Context>& context, World& world, float camera_width, float camera_height)
    : context(context),
      world(world),
      camera(Camera(sf::Vector2f(world.width / 2, world.height / 2), camera_width, camera_height)),
      spatial_boid_grid(SpatialGrid(world.size().cast<int>(), static_cast<int>(PERCEPTION_RADIUS/4))) {
};


void Simulation::AddBoid(const std::shared_ptr<Boid> &boid) {
    boids.push_back(boid);
    spatial_boid_grid.AddBoid(boid);
}

void Simulation::Init() {
    //Create Random boids
    int boids_amount = 1000;
    for (int i = 0; i < boids_amount; ++i) {
        auto boid = std::make_shared<Boid>(CreateRandomBoid(world.width, world.height));
        AddBoid(boid);
    }
};


void Simulation::Update(sf::Time delta_time) {

    for (const auto& boid: boids) {

        //Get boids in perception radius:
        auto perceived_boids = spatial_boid_grid.RadiusSearch(boid, boid->perception_radius);

        //Update boids acceleration
        const Eigen::Vector2f& aceleration = boid->CalcAcceleration(perceived_boids, obstacles, world);
        boid->SetAcceleration(aceleration);

        //Update boids language
        //boid->UpdateLanguage(perceived_boids);
    }

    for (const auto& boid : boids) {
        //Update boids velocity and position
        boid->SetVelocity(boid->vel + boid->acc * delta_time.asSeconds());
        boid->SetPosition(boid->pos + boid->vel * delta_time.asSeconds());
        spatial_boid_grid.UpdateBoid(boid);

        //Update boids sprite
        boid->UpdateSprite();
    }
};


void Simulation::ProcessInput() {

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    sf::Vector2f world_pos = context->window->mapPixelToCoords(mouse_pos);

    sf::Event event;
    while(context->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            context->window->close();
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            camera.StartDragging(world_pos);
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            camera.StopDragging();
        }
    }

    // std::cout << mouse_pos.x << world_pos.y << std::endl;
    // std::cout << context->window->getSize().x << std::endl;
    camera.Drag(world_pos);
};


void Simulation::Draw() {
    context->window->clear(sf::Color::Black);
    //context->window->setView(camera.view);
    for (const auto& boid : boids) {
        context->window->draw(boid->sprite);
    }

    sf::CircleShape circle(100); // Create a circle shape with radius 100
    circle.setFillColor(sf::Color::Blue); // Set the fill color to blue
    circle.setPosition(400, 300); // Set position to the center of the window
    context->window->draw(circle);

    context->window->display();
}


void Simulation::Start() {

};


void Simulation::Pause() {

};


Boid CreateRandomBoid(const int max_x_coord, const int max_y_coord, bool random_language) {
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