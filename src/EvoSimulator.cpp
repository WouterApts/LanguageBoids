//
// Created by wouter on 27-2-2024.
//

#include <random>
#include <iostream>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "Simulator.h"
#include "Utility.h"



EvoSimulator::EvoSimulator(std::shared_ptr<Context>& context, VectorSimulationData& simulation_data, std::string simulation_name,
                           float camera_width, float camera_height)
    : Simulator(context, simulation_data.config, simulation_data.world, camera_width, camera_height),
      boid_spawners(simulation_data.boid_spawners),
      num_threads(std::max(std::thread::hardware_concurrency() - 1.f, 1.f)),
      spatial_boid_grid(SpatialGrid<EvoBoid>(world.size().cast<int>(), static_cast<int>(config->INTERACTION_RADIUS))),
      output_file_path("output/" + simulation_name + "_output.txt") {

    // Create text for displaying the language of selecetd boid
    selected_boid_language_display.setFont(*ResourceManager::GetFont("arial"));
    selected_boid_language_display.setCharacterSize(20);
    selected_boid_language_display.setFillColor(sf::Color::White);
    selected_boid_language_display.setPosition(10.f, 10.f);
}


void EvoSimulator::Init() {
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

    //Create analyser for logging metrics
    analyser = std::make_shared<EvoAnalyser>(boids);
    analyser->SetLogTimeInterval(sf::seconds(config->ANALYSIS_LOG_INTERVAL));
};

void EvoSimulator::Update(sf::Time delta_time) {
    if (delta_time < sf::seconds(1 / 30.f)) {
        delta_time = sf::seconds(1 / 30.f);
    }

    if (config->MULTI_THREADING) {
        MultiThreadUpdate(delta_time);
    } else {
        std::cerr << "Evolution Simulator only supported with MULTI_THREADING enabled, for now.";
        context->state_manager->PopState();
    }

    // Update boids color if a boid is selected to compare with.
    if (selected_boid) {
        Eigen::VectorXf distances = dynamic_cast<EvoBoid*>(selected_boid)->CalcLanguageDistances(boids);
        for (int i = 0; i < boids.size(); ++i) {
            boids[i]->sprite.setColor(CalculateGradientColor(distances[i]));
        }
    }

    // Save metrics
    analyser->SaveMetricsToCSV(output_file_path, delta_time);
}

void EvoSimulator::MultiThreadUpdate(sf::Time delta_time) {
    std::vector<std::future<EvoSimulator::BoidValueMap>> future_pool;

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
    std::map<EvoBoid *, std::shared_ptr<BoidValues>> all_values;
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

    UpdateBoidsStepTwo(delta_time);

    //Handle boids life and death cycle
    RemoveDeadBoidsAndAddOffspring(all_values);
}

EvoSimulator::BoidValueMap EvoSimulator::MultiThreadUpdateStepOne(
    int start_index, int end_index, sf::Time delta_time) const {

    //UpdatedBoidValues boid_values;
    BoidValueMap UpdatedBoidValuesPerBoid;

    for (int i = start_index; i < end_index; ++i) {
        std::vector<EvoBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boids[i]->interaction_radius, boids[i]);
        std::vector<EvoBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boids[i]->perception_radius, boids[i]);
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
                //boidValuesPtr->most_common_language = boids[i]->GetMostCommonLanguage(interacting_boids);
                if (interacting_boids.size() > 0) {
                    r = GetRandomIntBetween(0,interacting_boids.size()-1);
                    boidValuesPtr->most_common_language = interacting_boids[r]->language_vector;
                } else {
                    boidValuesPtr->most_common_language = boids[i]->language_vector;
                }
            }
        }

        // Insert the unique_ptr into the map
        UpdatedBoidValuesPerBoid[boids[i].get()] = boidValuesPtr;
    }

    return UpdatedBoidValuesPerBoid;
    // return boid_values;
}


void EvoSimulator::UpdateBoidsStepTwo(sf::Time delta_time) {
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

void EvoSimulator::ProcessInput() {

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

void EvoSimulator::DrawWorldAndBoids() {
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

void EvoSimulator::DrawSpawners() const {
    // Set camera view
    context->window->setView(camera.view);

    // Draw Spawners
    for (const auto& spawners : boid_spawners) {
        spawners->Draw(context->window.get());
    }

    // Reset camera view to default
    context->window->setView(context->window->getDefaultView());
}

void EvoSimulator::Draw() {
    context->window->clear(sf::Color::Black);
    DrawWorldAndBoids();

    if (selected_boid) {
        std::stringstream ss;
        ss << "Boid Language: [" << dynamic_cast<EvoBoid*>(selected_boid)->language_vector.transpose() << "]";
        ss << "\nAge: " << static_cast<int>(dynamic_cast<EvoBoid*>(selected_boid)->age);
        selected_boid_language_display.setString(ss.str());
        context->window->draw(selected_boid_language_display);
    }

    context->window->display();
}

void EvoSimulator::Start() {

};


void EvoSimulator::Pause() {

};

void EvoSimulator::AddBoid(const std::shared_ptr<EvoBoid> &boid) {
    boids.push_back(boid);           // creates a copy of shared_ptr, assigning an additional owner (boids)
    spatial_boid_grid.AddObj(boid);
}

void EvoSimulator::RemoveDeadBoidsAndAddOffspring(BoidValueMap updated_boid_values) {

    std::vector<std::shared_ptr<EvoBoid>> offspring_boids;

    // Iterate through the boids and remove the ones marked for death, while also collecting offsprinf
    for (auto it = boids.begin(); it != boids.end();) {
        auto boidPtr = it->get();
        if (updated_boid_values[boidPtr]->marked_for_death) {

            // Add one offspring boid
            auto offspring_spawn_point = boidPtr->GetOffspringPos(world);
            auto new_boid = std::make_shared<EvoBoid>(offspring_spawn_point, Eigen::Vector2f::Zero(), Eigen::Vector2f::Zero(), config,
                                                         updated_boid_values[boidPtr]->most_common_language, 1);
            offspring_boids.emplace_back(std::move(new_boid));

            // Remove dead boid
            spatial_boid_grid.RemoveObj(*it);
            it = boids.erase(it);
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

