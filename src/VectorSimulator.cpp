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


// VectorSimulator::UpdatedBoidValues VectorSimulator::UpdateBoidsStepOneMultithread(const std::vector<std::shared_ptr<VectorBoid>> &boids, sf::Time delta_time) const {
//
//     UpdatedBoidValues updated_boid_values;
//     for (const auto& boid : boids) {
//         //Get boids in perception radius:
//         std::vector<VectorBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid);
//         std::vector<VectorBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boid->perception_radius, boid);
//         Eigen::VectorXf language_distances = boid->CalcLanguageDistances(interacting_boids);
//
//         //Get boids updated acceleration
//         updated_boid_values.acceleration_values[boid.get()] = boid->GetUpdatedAcceleration(interacting_boids, language_distances);
//
//         //Get boids language features that haev to be updated (switched)
//         //updated_boid_values.language_features[boid.get()] = boid->GetUpdatedLanguageFeatures(interacting_boids, language_distances, perceived_boids, delta_time);
//
//         //Get boids updated age
//         float new_age = boid->age += delta_time.asSeconds();
//         updated_boid_values.age_values[boid.get()] = new_age;
//
//         // If boid is too old, it has a chance of dying
//         if (new_age >= config->BOID_LIFE_STEPS) {
//             updated_boid_values.marked_for_death[boid.get()] = true;
//             float r = GetRandomFloatBetween(0,1);
//             if (r <= 0.1) {
//                 // The language of it's offspring is calculated as the most common language in the interaction range:
//                 updated_boid_values.most_common_language[boid.get()] = boid->GetMostCommonLanguage(interacting_boids);
//             }
//         }
//     }
//
//     return updated_boid_values;
// }

void VectorSimulator::Update(sf::Time delta_time) {

    if (config->MULTI_THREADING) {
        MultiThreadUpdate(delta_time);
    }

    // Update boids color if a boid is selected to compare with.
    if (selected_boid) {
        Eigen::VectorXf distances = dynamic_cast<VectorBoid*>(selected_boid)->CalcLanguageDistances(boids);
        for (int i = 0; i < boids.size(); ++i) {
            boids[i]->sprite.setColor(CalculateGradientColor(distances[i]));
        }
    }
}

void VectorSimulator::MultiThreadUpdate(sf::Time delta_time) {

    std::vector<std::future<UpdatedBoidValues>> future_pool;

    // Launch threads, each thread handling a chunk of Boids
    int elements_per_chunk = boids.size() / num_threads;
    int start_indices[num_threads];
    int end_indices[num_threads];
    for (size_t i = 0; i < num_threads; ++i) {
        start_indices[i] = elements_per_chunk * i;
        end_indices[i] = elements_per_chunk * (i + 1);
        if (i == num_threads - 1) {
            end_indices[i] = boids.size();
        }
        //std::cout << start_indices[i] << "->" << end_indices[i] << std::endl;
        int start_index = start_indices[i];
        int end_index = end_indices[i];
        future_pool.emplace_back(std::async(std::launch::async, [=]() {
            return MultiThreadUpdateStepOne(start_index, end_index, delta_time);
        }));
    }

    // Wait for all threads to finish
    UpdatedBoidValues all_values;
    for (auto& future : future_pool) {
        auto partion = future.get();
        std::lock_guard<std::mutex> lock(mtx);
        // Collect partions and add them together
        all_values.acceleration_values.insert(partion.acceleration_values.begin(), partion.acceleration_values.end());
        all_values.language_features.insert(partion.language_features.begin(), partion.language_features.end());
    }

    //Set updated accelration and language feature
    for (auto& boid : boids) {
        boid->SetAcceleration(all_values.acceleration_values[boid.get()]);
        boid->SwitchLanguageFeatures(all_values.language_features[boid.get()]);
    }

    // Update velocities, positions and sprites
    // std::vector<std::future<void>> future_pool_2;
    // for (size_t i = 0; i < num_threads; ++i) {
    //     int start_index = start_indices[i];
    //     int end_index = end_indices[i];
    //     future_pool_2.emplace_back(std::async(std::launch::async, [=]() {
    //         MultiThreadUpdateStepTwo(start_index, end_index, delta_time);
    //     }));
    // }

    // Update velocities, positions and sprites
    UpdateBoidsStepTwo(delta_time);
}

VectorSimulator::UpdatedBoidValues VectorSimulator::MultiThreadUpdateStepOne(int start_index, int end_index, sf::Time delta_time) const {
    UpdatedBoidValues boid_values;

    for (int i = start_index; i < end_index; ++i) {
        std::vector<VectorBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boids[i]->interaction_radius, boids[i]);
        std::vector<VectorBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boids[i]->perception_radius, boids[i]);
        Eigen::VectorXf language_distances = boids[i]->CalcLanguageDistances(interacting_boids);

        boid_values.acceleration_values[boids[i].get()] = (boids[i]->GetUpdatedAcceleration(interacting_boids, language_distances));
        boid_values.language_features[boids[i].get()]   = (boids[i]->GetUpdatedLanguageFeatures(interacting_boids,
                                                                                                    language_distances,
                                                                                                    perceived_boids,
                                                                                                    delta_time));
    }

    return boid_values;
}


void VectorSimulator::MultiThreadUpdateStepTwo(int start_index, int end_index, sf::Time delta_time) {
    for (int i = start_index; i < end_index; ++i) {

        //Update boid behaviour based on terrain effects
        bool in_terrain = false;
        for (auto& terrain : world.terrains) {
            if (terrain->IsPointInside(boids[i]->pos)) {
                terrain->ApplyEffects(boids[i].get());
                in_terrain = true;
            }
        }
        if (!in_terrain) boids[i]->SetDefaultMinMaxSpeed();

        //Update boids velocity (Also checking Collisions)
        boids[i]->UpdateVelocity(world.obstacles, delta_time);

        //Update boids position
        boids[i]->UpdatePosition(delta_time);
        spatial_boid_grid.UpdateObj(boids[i]);

        //Update boids sprite
        boids[i]->UpdateSprite();
    }
}

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

void VectorSimulator::AddBoid(const std::shared_ptr<VectorBoid> &boid) {
    boids.push_back(boid);           // creates a copy of shared_ptr, assigning an additional owner (boids)
    spatial_boid_grid.AddObj(boid);
}

