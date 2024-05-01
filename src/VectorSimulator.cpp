//
// Created by wouter on 27-2-2024.
//

#include <random>
#include <iostream>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "Simulator.h"
#include "Utility.h"

VectorSimulator::VectorSimulator(std::shared_ptr<Context>& context, VectorSimulationData& simulation_data, float camera_width, float camera_height)
    : Simulator(context, simulation_data.config, simulation_data.world, camera_width, camera_height),
      boid_spawners(simulation_data.boid_spawners),
      num_threads(std::thread::hardware_concurrency()),
      spatial_boid_grid(SpatialGrid<VectorBoid>(world.size().cast<int>(), static_cast<int>(config->INTERACTION_RADIUS))) {}


void VectorSimulator::Init() {
    // Setup analysis logging

    // Setup world borders
    CreateWorldBorderLines();

    // Fit Camera view to world
    camera.FitWorld(world);

    // Spawn Boids
    for (auto& spawner : boid_spawners) {
        spawner->AddBoids(world, config, boids);
    }

    // Initialize boids in spatial grid
    for (auto& boid : boids) {
        spatial_boid_grid.AddObj(boid);
    }
};


VectorSimulator::UpdatedBoidValues VectorSimulator::UpdateBoidsStepOneMultithread(const std::vector<std::shared_ptr<VectorBoid>> &boids, sf::Time delta_time) const {

    UpdatedBoidValues updated_boid_values;
    for (const auto& boid : boids) {
        //Get boids in perception radius:
        std::vector<VectorBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid);
        std::vector<VectorBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boid->perception_radius, boid);
        Eigen::VectorXf language_distances = boid->CalcLanguageDistances(interacting_boids);

        //Get boids updated acceleration
        updated_boid_values.acceleration_values[boid.get()] = boid->GetUpdatedAcceleration(interacting_boids, language_distances);

        //Get boids language features that haev to be updated (switched)
        updated_boid_values.language_features[boid.get()] = boid->GetUpdatedLanguageFeatures(interacting_boids,
                                                                                             language_distances,
                                                                                             perceived_boids,
                                                                                             delta_time);
        //Get boids updated age
        float new_age = boid->age += delta_time.asSeconds();
        updated_boid_values.age_values[boid.get()] = new_age;

        // If boid is too old, it has a chance of dying
        if (new_age >= config->BOID_LIFE_STEPS) {
            updated_boid_values.marked_for_death[boid.get()] = true;
            float r = GetRandomFloatBetween(0,1);
            if (r <= 0.1) {
                // The language of it's offspring is calculated as the most common language in the interaction range:
                updated_boid_values.most_common_language[boid.get()] = boid->GetMostCommonLanguage(interacting_boids);
            }
        }
    }

    return updated_boid_values;
}

void DivideIntoChunks(const std::vector<std::shared_ptr<VectorBoid>>& boids, size_t num_threads, std::vector<std::vector<std::shared_ptr<VectorBoid>>>& boid_chunks) {
    size_t total_boids = boids.size();
    size_t chunk_size = std::ceil(static_cast<double>(total_boids) / num_threads);

    size_t start_index = 0;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t end_index = std::min(start_index + chunk_size, total_boids);
        boid_chunks.emplace_back(boids.begin() + start_index, boids.begin() + end_index);
        start_index = end_index;
        if (start_index >= total_boids) {
            break;
        }
    }
}

void VectorSimulator::Update(sf::Time delta_time) {

    if (config->MULTI_THREADING) {
        std::vector<std::future<UpdatedBoidValues>> future_pool;
        std::vector<std::vector<std::shared_ptr<VectorBoid>>> boid_chunks;
        DivideIntoChunks(boids, num_threads, boid_chunks);

        // Launch threads
        for (size_t i = 0; i < num_threads; ++i) {
            future_pool.emplace_back(std::async(std::launch::async, [&](size_t thread_id) {
                return UpdateBoidsStepOneMultithread(boid_chunks[thread_id], delta_time);
            }, i));
        }

        // Collect and combine partial results
        UpdatedBoidValues all_updated_values;
        for (auto& future : future_pool) {
            UpdatedBoidValues partial = future.get();
            // Combine partial results
            std::lock_guard<std::mutex> lock(mtx);
            for (const auto& entry : partial.acceleration_values) {
                all_updated_values.acceleration_values.insert(entry);
            }
            for (const auto& entry : partial.language_features) {
                all_updated_values.language_features.insert(entry);
            }
            for (const auto& entry : partial.age_values) {
                all_updated_values.age_values.insert(entry);
            }
            for (const auto& entry : partial.most_common_language) {
                all_updated_values.most_common_language.insert(entry);
            }
        }

        // Apply value updates to boids and mark for death or create new boids
        std::vector<std::shared_ptr<VectorBoid>> offspring_boids;
        for (auto& boid : boids) {
            if (!all_updated_values.marked_for_death[boid.get()]) {
                // Acceleration
                auto& acceleration_value = all_updated_values.acceleration_values[boid.get()];
                boid->SetAcceleration(acceleration_value);

                // Language features
                boid->SwitchLanguageFeatures(all_updated_values.language_features[boid.get()]);

                // Age
                boid->age = all_updated_values.age_values[boid.get()];

                offspring_boids.push_back(boid); // Keep the boid
            }
        }

        // Delete marked boids
        boids.erase(std::remove_if(boids.begin(), boids.end(), [&](const std::shared_ptr<VectorBoid>& boid) {
            return all_updated_values.marked_for_death[boid.get()];
        }), boids.end());

        // Add new boids
        for (auto& offspring : offspring_boids) {
            boids.push_back(offspring);
        }
    }

    UpdateBoidsStepTwo(delta_time);

    // Update boids color if a boid is selected to compare with.
    if (selected_boid) {
        Eigen::VectorXf distances = dynamic_cast<VectorBoid*>(selected_boid)->CalcLanguageDistances(boids);
        for (int i; i < boids.size(); ++i) {
            boids[i]->sprite.setColor(CalculateGradientColor(distances[i]));
        }
    }
};

void VectorSimulator::UpdateBoidsStepTwo(sf::Time delta_time) {
    for (const auto& boid : boids) {

        //Update boid behaviour based on terrain effects
        bool in_terrain = false;
        for (auto& terrain : world.terrains) {
            if (terrain->IsPointInside(boid->pos)) {
                terrain->ApplyEffects(boid.get());
                in_terrain = true;
            }
        }
        if (!in_terrain) boid->SetDefaultMinMaxSpeed();

        //Update boids velocity (Also checking Collisions)
        boid->UpdateVelocity(world.obstacles, delta_time);

        //Update boids position
        boid->UpdatePosition(delta_time);
        spatial_boid_grid.UpdateObj(boid);

        //Update boids sprite
        boid->UpdateSprite();

    }
}

void VectorSimulator::ProcessInput() {

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    sf::Event event{};
    while(context->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            context->window->close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                //Boid Selection
                ProcessBoidSelection(context.get(), mouse_pos, spatial_boid_grid);
            }
            if (event.mouseButton.button == sf::Mouse::Middle) {
                //Camera Drag
                camera.StartDragging(mouse_pos);
            }
        }

        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Middle) {
                //Camera Drag
                camera.StopDragging();
            }
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            //Camera Zoom
            ProcessCameraZoom(event);
        }

        if (IsKeyPressedOnce(sf::Keyboard::G)) {
            std::cout << "Visual Spatial Grid:" << !spatial_boid_grid.is_visible << std::endl;
            spatial_boid_grid.is_visible = !spatial_boid_grid.is_visible;
        }

        if (IsKeyPressedOnce(sf::Keyboard::F5)) {
            //TODO: Save State?
        }

        if (IsKeyPressedOnce(sf::Keyboard::Escape)) {
            context->state_manager->PopState();
        }
    }
    camera.Drag(mouse_pos);
};

void VectorSimulator::DrawWorldAndBoids() {
    // Set camera view
    context->window->setView(camera.view);

    // Draw Terrain
    for (const auto& terrain : world.terrains) {
        terrain->Draw(context->window.get());
    }

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

    // Reset camera view to default
    context->window->setView(context->window->getDefaultView());
}

void VectorSimulator::DrawSpawners() const {
    // Set camera view
    context->window->setView(camera.view);

    // Draw Spawners
    for (const auto& spawners : boid_spawners) {
        spawners->Draw(context->window.get());
    }

    // Reset camera view to default
    context->window->setView(context->window->getDefaultView());
}

void VectorSimulator::Draw() {
    context->window->clear(sf::Color::Black);
    DrawWorldAndBoids();
    context->window->display();
}

void VectorSimulator::Start() {

};


void VectorSimulator::Pause() {

};


// VectorBoid VectorSimulator::CreateRandomBoid(float max_x_coord, float max_y_coord, bool random_language) {
//     // Generate random position
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_real_distribution<float> disX(0, static_cast<float>(max_x_coord));
//     std::uniform_real_distribution<float> disY(0, static_cast<float>(max_y_coord));
//     Eigen::Vector2f position(disX(gen), disY(gen));
//
//     // Generate random velocity and acceleration
//     std::uniform_real_distribution<float> disX2(MIN_SPEED, MAX_SPEED);
//     std::uniform_real_distribution<float> disY2(MIN_SPEED, MAX_SPEED);
//     Eigen::Vector2f velocity(disX2(gen), disY2(gen));
//     Eigen::Vector2f acceleration(disX2(gen), disY2(gen));
//
//     // Generate random language
//     Eigen::VectorXi language = Eigen::VectorXi::Ones(LANGUAGE_SIZE);
//     if (random_language) {
//         std::uniform_int_distribution distrubution_language(0, 1);
//         for (int i = 0; i < LANGUAGE_SIZE; ++i) {
//             language(i) = distrubution_language(gen);
//         }
//     }
//
//     float language_influence = 1.0f;
//
//     // Create and return the Boid object
//     return {position, velocity, acceleration, language, language_influence};
// }

void VectorSimulator::AddBoid(const std::shared_ptr<VectorBoid> &boid) {
    boids.push_back(boid);           // creates a copy of shared_ptr, assigning an additional owner (boids)
    spatial_boid_grid.AddObj(boid);
}

