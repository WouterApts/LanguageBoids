//
// Created by wouter on 19-2-2024.
//

#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <SFML/Graphics/View.hpp>

#include "Boid.h"
#include "State.h"
#include "Application.h"
#include "Camera.h"
#include "Obstacles.h"
#include "SpatialGrid.h"

class Simulation : public State {
public:

    Simulation(std::shared_ptr<Context> &context, World &world, float camera_width, float camera_height);

    std::shared_ptr<Context> context;
    World world;
    Camera camera;
    std::vector<std::shared_ptr<Boid>> boids;
    SpatialGrid spatial_boid_grid;
    std::vector<std::shared_ptr<Obstacle>> obstacles;

    void AddBoid(const std::shared_ptr<Boid> &boid);

    void Init() override;

    void Update(sf::Time delta_time) override;

    void ProcessInput() override;

    void Draw() override;

    void Start() override;

    void Pause() override;
};

Boid CreateRandomBoid(int max_x_coord, int max_y_coord, bool random_language = false);

#endif //SIMULATION_H
