//
// Created by wouter on 19-2-2024.
//

#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>

#include "Boid.h"
#include "State.h"
#include "Camera.h"
#include "analysis/KeyAnalyser.h"
#include "LanguageManager.h"
#include "Obstacles.h"
#include "SimulationData.h"
#include "SpatialGrid.tpp"
#include "Application.h"

class Simulator : public State {
public:
    std::shared_ptr<Context> context;
    std::shared_ptr<SimulationConfig> config;
    World world;
    Camera camera;
    Boid* selected_boid;
    sf::Sprite boid_selection_border;
    std::shared_ptr<sf::Texture>  boid_selection_texture;
    float total_simulation_time = 0.f;

    Simulator(std::shared_ptr<Context> &context, std::shared_ptr<SimulationConfig>& config, World &world, float camera_width, float camera_height);

    // ProcessInput Methods
    template <typename BoidType>
    void ProcessBoidSelection(const Context* context, sf::Vector2i& mouse_pos, SpatialGrid<BoidType>& spatial_boid_grid);

    void ProcessCameraZoom(const sf::Event &event);

    // Draw methods
    void DrawBoidSelectionCircle();

    void CreateWorldBorderLines();
};

class VectorSimulator : public Simulator {
public:

    struct UpdatedBoidValues {
        std::map<VectorBoid*, Eigen::Vector2f> acceleration_values;
        std::map<VectorBoid*, std::set<int>> language_features;
        std::map<VectorBoid*, bool> marked_for_death;
        std::map<VectorBoid*, Eigen::VectorXi> most_common_language;
    };

    struct BoidValues {
        BoidValues() = default;
        Eigen::Vector2f acceleration_value;
        std::set<int> language_features;
        bool marked_for_death = false;
        Eigen::VectorXi most_common_language;
    };

    using BoidValueMap = std::map<VectorBoid*, std::shared_ptr<BoidValues>>;

    SpatialGrid<VectorBoid> spatial_boid_grid;
    std::vector<std::shared_ptr<VectorBoid>> boids;
    std::vector<std::shared_ptr<VectorBoidSpawner>> boid_spawners;
    int total_boids = 0;
    sf::Text total_boids_display_text;

    // Multi-Threading
    std::mutex mtx;
    const size_t num_threads;

    VectorSimulator(std::shared_ptr<Context>& context, VectorSimulationData& simulation_data, float camera_width, float camera_height);

    void Init() override;
    void AddBoid(const std::shared_ptr<VectorBoid> &boid);

    void Update(sf::Time delta_time) override;
    void MultiThreadUpdate(sf::Time delta_time);

    BoidValueMap MultiThreadUpdateStepOne(int start_index, int end_index, sf::Time delta_time) const;
    void RemoveDeadBoidsAndAddOffspring(std::map<VectorBoid *, std::shared_ptr<BoidValues>> updated_boid_values
    );

    void MultiThreadUpdateStepTwo(int start_index, int end_index, sf::Time delta_time);

    void UpdateBoidsStepTwo(sf::Time delta_time);
    void ProcessInput() override;

    void DrawWorldAndBoids();
    void DrawSpawners() const;

    void Draw() override;
    void Start() override;
    void Pause() override;


};


class KeySimulator : public Simulator {
public:

    struct UpdatedBoidValues {
        std::map<KeyBoid*, Eigen::Vector2f> acceleration_values;
        std::map<KeyBoid*, std::pair<int, float>> language_and_satisfaction_values;
    };

    SpatialGrid<KeyBoid> spatial_boid_grid;
    std::vector<std::shared_ptr<KeyBoid>> boids;
    std::vector<std::shared_ptr<KeyBoidSpawner>> boid_spawners;

    // Multi-Threading
    std::mutex mtx;
    const size_t num_threads;
    std::vector<std::vector<std::shared_ptr<KeyBoid>>> boid_chunks;

    // KeyAnalyser analyser;
    LanguageManager language_manager;
    std::shared_ptr<std::map<int, float>> default_languages_status_map;

    KeySimulator(std::shared_ptr<Context>& context, KeySimulationData& simulation_data, float camera_width, float camera_height);

    void Init() override;

    void DivideBoidChunks();

    UpdatedBoidValues UpdateBoidsStepOneMultithread(const std::vector<std::shared_ptr<KeyBoid> > &boids,
                                                                  sf::Time delta_time) const;

    void UpdateBoidsStepOne(const std::vector<std::shared_ptr<KeyBoid>> &boids, sf::Time delta_time) const;
    void UpdateBoidsStepTwo(const std::vector<std::shared_ptr<KeyBoid>> &boids, sf::Time delta_time);
    void Update(sf::Time delta_time) override;

    void ProcessInput() override;

    void DrawWorldAndBoids();
    void DrawSpawners() const;

    void Draw() override;
    void Start() override;
    void Pause() override;

    void AddBoid(const std::shared_ptr<KeyBoid> &boid);

};

#endif //SIMULATION_H
