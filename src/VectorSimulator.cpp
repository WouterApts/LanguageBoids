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
      spatial_boid_grid(SpatialGrid<VectorBoid>(world.size().cast<int>(), static_cast<int>(config->INTERACTION_RADIUS))) {

    // Create total_boids textdisplay tracker
    total_boids_display_text.setFont(*ResourceManager::GetFont("arial"));
    total_boids_display_text.setCharacterSize(24);
    total_boids_display_text.setFillColor(sf::Color::White);
    total_boids_display_text.setPosition(10.f, 10.f);
}


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
    if (delta_time < sf::seconds(1/20.f)) { delta_time = sf::seconds(1/20.f); }

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
    std::vector<std::future<VectorSimulator::BoidValueMap>> future_pool;

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
    std::map<VectorBoid *, std::shared_ptr<BoidValues>> all_values;
    for (auto& future : future_pool) {
        auto partion = future.get();
        std::lock_guard<std::mutex> lock(mtx);
        // Collect partions and add them together
        all_values.insert(partion.begin(), partion.end());
    }

    //Set updated accelration and language features
    for (auto& boid : boids) {
        boid->SetAcceleration(all_values[boid.get()]->acceleration_value);
        boid->SwitchLanguageFeatures(all_values[boid.get()]->language_features);
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
    // for (auto& boid : boids) {
    //     spatial_boid_grid.UpdateObj(boid);
    // }

    UpdateBoidsStepTwo(delta_time);

    //Handle boids life and death cycle
    RemoveDeadBoidsAndAddOffspring(all_values);
}

VectorSimulator::BoidValueMap VectorSimulator::MultiThreadUpdateStepOne(
    int start_index, int end_index, sf::Time delta_time) const {
    //UpdatedBoidValues boid_values;
    VectorSimulator::BoidValueMap UpdatedBoidValuesPerBoid;

    for (int i = start_index; i < end_index; ++i) {
        std::vector<VectorBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boids[i]->interaction_radius, boids[i]);
        std::vector<VectorBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boids[i]->perception_radius, boids[i]);
        Eigen::VectorXf language_distances = boids[i]->CalcLanguageDistances(interacting_boids);


        auto boidValuesPtr = std::make_shared<BoidValues>();
        boidValuesPtr->acceleration_value = (boids[i]->GetUpdatedAcceleration(interacting_boids, language_distances));
        boidValuesPtr->language_features = (boids[i]->GetUpdatedLanguageFeatures(interacting_boids,
                                                                                   language_distances,
                                                                                   perceived_boids,
                                                                                   delta_time));

        if (boids[i]->age >= config->BOID_LIFE_STEPS) {
            float r = GetRandomFloatBetween(0,1);
            if (r <= 0.01 * delta_time.asSeconds()) {
                boidValuesPtr->marked_for_death = true;
                boidValuesPtr->most_common_language = boids[i]->GetMostCommonLanguage(interacting_boids);
            }
        }

        // Insert the unique_ptr into the map
        UpdatedBoidValuesPerBoid[boids[i].get()] = boidValuesPtr;
    }

    return UpdatedBoidValuesPerBoid;
    // return boid_values;
}


void VectorSimulator::MultiThreadUpdateStepTwo(int start_index, int end_index, sf::Time delta_time) {

    for (int i = start_index; i < end_index; ++i) {

        //Update boid behaviour based on terrain effects
        bool in_terrain = false;
        for (auto& terrain : world.terrains) {
            if (terrain->IsPointInside(boids[i]->pos)) {
                terrain->ApplyMovementEffects(boids[i].get());
                in_terrain = true;
            }
        }
        if (!in_terrain) boids[i]->SetDefaultMinMaxSpeed();

        //Update boids velocity (Also checking Collisions)
        boids[i]->UpdateVelocity(world.obstacles, delta_time);

        //Update boids position
        boids[i]->UpdatePosition(delta_time);

        //Update boid age
        boids[i]->age += delta_time.asSeconds();

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
                terrain->ApplyMovementEffects(boid.get());
                in_terrain = true;
            }
        }
        if (!in_terrain) boid->SetDefaultMinMaxSpeed();

        //Update boids velocity (Also checking Collisions)
        boid->UpdateVelocity(world.obstacles, delta_time);

        //Update boids position
        boid->UpdatePosition(delta_time);
        spatial_boid_grid.UpdateObj(boid);

        //Update boids age
        boid->age += delta_time.asSeconds();

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
                if (!selected_boid) {
                    for (auto& boid : boids) { boid->sprite.setColor(sf::Color::Yellow);}
                }
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

        if (IsKeyPressedOnce(sf::Keyboard::Space)) {
            std::unordered_map<Eigen::VectorXi, int, vectorXiHash, vectorXiEqual> language_counts;
            for (auto& boid : boids) {
                language_counts[boid->language_vector] += 1;
            }
            std::cout << "different languages globaly:" << language_counts.size() << std::endl;
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

    std::stringstream ss;
    ss << "Total Boids: " << total_boids;
    total_boids_display_text.setString(ss.str());
    context->window->draw(total_boids_display_text);

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

void VectorSimulator::RemoveDeadBoidsAndAddOffspring(BoidValueMap updated_boid_values) {

    std::vector<std::shared_ptr<VectorBoid>> offspring_boids;
    total_boids = boids.size();

    // Iterate through the boids and remove the ones marked for death, while also collecting offsprinf
    for (auto it = boids.begin(); it != boids.end();) {
        auto boidPtr = it->get();
        if (updated_boid_values[boidPtr]->marked_for_death) {

            // Add one or two offspring boids
            int n_offspring = 1;
            if (total_boids < config->CARRYING_CAPACITY) n_offspring = 2;
            for (int n = 0; n < n_offspring; ++n) {
                auto offspring_spawn_point = boidPtr->GetOffspringPos(world);
                auto new_boid = std::make_shared<VectorBoid>(offspring_spawn_point, Eigen::Vector2f::Zero(), Eigen::Vector2f::Zero(), config,
                                                             updated_boid_values[boidPtr]->most_common_language, 1);
                offspring_boids.emplace_back(std::move(new_boid));
                total_boids += 1;
            }

            // Remove dead boid
            spatial_boid_grid.RemoveObj(*it);
            it = boids.erase(it);
            total_boids -= 1;
            if (boidPtr == selected_boid) {
                selected_boid = nullptr;
                for (auto& boid : boids) { boid->sprite.setColor(sf::Color::Yellow);}
            }

        } else {
            ++it;
        }
    }

    // Add new offspring boids to boids vector and grid
    for (auto& boid :offspring_boids) {
        boids.push_back(boid);
        spatial_boid_grid.AddObj(boid);
    }
}

