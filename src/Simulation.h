//
// Created by wouter on 19-2-2024.
//

#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>

#include "Boid.h"
#include "State.h"
#include "Application.h"
#include "Camera.h"
#include "LanguageManager.h"
#include "Obstacles.h"
#include "SpatialGrid.tpp"


class Simulation : public State {
public:

    std::shared_ptr<Context> context;
    World world;
    Camera camera;
    std::vector<std::shared_ptr<Obstacle>> obstacles;
    Boid* selected_boid;

    Simulation(std::shared_ptr<Context> &context, World &world, float camera_width, float camera_height);

    // ProcessInput Methods
    template <typename BoidType>
    void ProcessBoidSelection(const Context* context, sf::Vector2i& mouse_pos, SpatialGrid<BoidType>& spatial_boid_grid);
    void ProcessCameraZoom(const sf::Event &event);

    // Draw methods
    void DrawBoidSelectionCircle() const;
    void DrawBorderOutline() const;

};

class EvoSimulation : public Simulation {
public:
    SpatialGrid<EvoBoid> spatial_boid_grid;
    std::vector<std::shared_ptr<EvoBoid>> boids;

    EvoSimulation(std::shared_ptr<Context> &context, World &world, float camera_width, float camera_height);

    void Init() override;
    void Update(sf::Time delta_time) override;
    void ProcessInput() override;

    void Draw() override;
    void Start() override;
    void Pause() override;

    void AddBoid(const std::shared_ptr<EvoBoid> &boid);
    static EvoBoid CreateRandomBoid(float max_x_coord, float max_y_coord, bool random_language);
};


class CompSimulation : public Simulation {
public:

    SpatialGrid<CompBoid> spatial_boid_grid;
    std::vector<std::shared_ptr<CompBoid>> boids;
    LanguageManager language_manager;
    sf::Time passedTime;

    CompSimulation(std::shared_ptr<Context> &context, World &world, const std::vector<float> &language_statuses,
                   float camera_width, float camera_height);

    void Init() override;
    void Update(sf::Time delta_time) override;
    void ProcessInput() override;
    void Draw() override;
    void Start() override;
    void Pause() override;

    void AddBoid(const std::shared_ptr<CompBoid> &boid);
    void AddBoidCluster(Eigen::Vector2f position, float radius, int ammount, int language_key);
};

#endif //SIMULATION_H
