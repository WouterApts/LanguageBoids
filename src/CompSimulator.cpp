//
// Created by wouter on 27-2-2024.
//

#include <iostream>
#include <memory>
#include <execution>

#include "Application.h"
#include "BoidSpawners.h"
#include "LanguageManager.h"
#include "Simulator.h"
#include "Utility.h"
#include "Serialization.h"

CompSimulator::CompSimulator(std::shared_ptr<Context>& context, KeySimulationData& simulation_data, float camera_width, float camera_height)
    : Simulator(context, simulation_data.config, simulation_data.world, camera_width, camera_height),
      boid_spawners(simulation_data.boid_spawners),
      num_threads(std::thread::hardware_concurrency()),
      spatial_boid_grid(SpatialGrid<CompBoid>(world.size().cast<int>(), static_cast<int>(config->INTERACTION_RADIUS)))
{
    std::map<int, int> languages;
    std::map<int, float> default_languages_status_map;
    for (auto &spawner: boid_spawners) {
        languages[spawner->language_key] += 1;
        default_languages_status_map[spawner->language_key] = 1;  // Create default status map (all languages are perceived equaly by default)
    }
    language_manager = LanguageManager(static_cast<int>(languages.size()));
    this->default_languages_status_map = std::make_shared<std::map<int, float>>(default_languages_status_map);
    
    for (auto& terrain : world.terrains) {
        // Initialize status maps for different terrain zones (possibly increasing/decreasing a language's base status)
        terrain->InitLanguageStatusMap(default_languages_status_map);
    }

    // Setup analysis logging if enebled
    if (config->LANGUAGE_LOG_INTERVAL > 0 || config->POSITION_LOG_INTERVAL > 0) {
        analyser = std::make_unique<CompAnalyser>(boids);
        analyser->SetBPLTimeInterval(sf::seconds(config->LANGUAGE_LOG_INTERVAL));
        analyser->SetPPLTimeInterval(sf::seconds(config->POSITION_LOG_INTERVAL));
    }
}

void CompSimulator::Init() {

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

    for (auto& boid : boids) {
        boid->UpdateColor();
        boid->SetLanguageStatusMap(default_languages_status_map);
    }

    // Divide boids into chunks for multi-threading
    if (config->MULTI_THREADING) {
        DivideBoidChunks();
    }
}

void CompSimulator::DivideBoidChunks() {
    // Split the boids vector into chunks for each thread
    boid_chunks.resize(num_threads);
    for (size_t i = 0; i < boids.size(); ++i) {
        boid_chunks[i % num_threads].push_back(boids[i]);
    }
}

CompSimulator::UpdatedBoidValues CompSimulator::UpdateBoidsStepOneMultithread(const std::vector<std::shared_ptr<CompBoid> > &boids, sf::Time delta_time) const {

    UpdatedBoidValues updated_boid_values;
    for (const auto& boid : boids) {
        //Get boids in perception radius:
        std::vector<CompBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid);
        std::vector<CompBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boid->perception_radius, boid);

        //Update boids acceleration
        updated_boid_values.acceleration_values[boid.get()] = boid->GetUpdatedAcceleration(interacting_boids);

        //Update boids language satisfaction
        updated_boid_values.language_and_satisfaction_values[boid.get()] = boid->GetUpdatedLanguageAndSatisfaction(perceived_boids,
                                                                                                                   interacting_boids,
                                                                                                                   delta_time);
    }
    return updated_boid_values;
}

void CompSimulator::UpdateBoidsStepOne(const std::vector<std::shared_ptr<CompBoid>>& boids, sf::Time delta_time) const {
    for (const auto& boid : boids) {

        //Get boids in perception radius:
        std::vector<CompBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid);
        std::vector<CompBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boid->perception_radius, boid);

        //Update boids acceleration
        boid->UpdateAcceleration(interacting_boids);

        //Update boids language satisfaction
        boid->UpdateLanguageSatisfaction(perceived_boids, interacting_boids, delta_time);
    }
}

void CompSimulator::UpdateBoidsStepTwo(const std::vector<std::shared_ptr<CompBoid>>& boids, sf::Time delta_time) {
    for (const auto& boid : boids) {

        //Update boid behaviour based on terrain effects
        bool in_terrain = false;
        for (auto& terrain : world.terrains) {
            if (terrain->IsPointInside(boid->pos)) {
                terrain->ApplyMovementEffects(boid.get());
                terrain->ApplyLanguageStatusEffects(boid.get());
                in_terrain = true;
            }
        }
        if (!in_terrain) {
            boid->SetDefaultMinMaxSpeed();
            boid->SetLanguageStatusMap(default_languages_status_map);
        }

        //Update boids velocity (Also checking Collisions)
        boid->UpdateVelocity(world.obstacles, delta_time);

        //Update boids position
        boid->UpdatePosition(delta_time);
        spatial_boid_grid.UpdateObj(boid);

        //Update boids language
        //TODO: split multi-thread and single thread, this function is useless in multi (updated_language_key is always -1)
        boid->UpdateLanguage();

        //Update boids sprite
        boid->UpdateSprite();
    }
}

// Log metrics based on interval settings
//analyser.LogAllMetrics(delta_time);
void CompSimulator::Update(sf::Time delta_time) {

    if (speed_up_sumlation) {
        if (delta_time < sf::seconds(1/30.f)) { delta_time = sf::seconds(1/30.f); }
    }

    if (config->MULTI_THREADING) {
        std::vector<std::future<UpdatedBoidValues>> future_pool;
        std::vector<UpdatedBoidValues> partial_values(num_threads);

        // Launch threads
        for (size_t i = 0; i < num_threads; ++i) {
            future_pool.emplace_back(std::async(std::launch::async, [&](size_t thread_id) {
                return UpdateBoidsStepOneMultithread(boid_chunks[thread_id], delta_time);
            }, i));
        }

        // Collect partial results from futures
        std::mutex resultsMutex;
        for (size_t i = 0; i < num_threads; ++i) {
            std::lock_guard lock(resultsMutex);
            partial_values[i] = future_pool[i].get();
        }

        // Combine partial results
        UpdatedBoidValues all_updated_values;
        for (const auto& partial : partial_values) {
            for (const auto& entry : partial.acceleration_values) {
                all_updated_values.acceleration_values.insert(entry);
            }
            for (const auto& entry : partial.language_and_satisfaction_values) {
                all_updated_values.language_and_satisfaction_values.insert(entry);
            }
        }

        // Apply value updates to boids
        for (auto& boid : boids) {
            auto& acceleration_value = all_updated_values.acceleration_values[boid.get()];
            boid->SetAcceleration(acceleration_value);
            auto& language_and_satisfaction_value = all_updated_values.language_and_satisfaction_values[boid.get()];
            boid->SetLanguageKey(language_and_satisfaction_value.first);
            boid->SetLanguageSatisfaction(language_and_satisfaction_value.second);

        }
    } else {
        UpdateBoidsStepOne(boids, delta_time);
    }

    // Update boids position, language, and sprite sequentially
    UpdateBoidsStepTwo(boids, delta_time);

    // Log analysis data if analysing is enabled
    if (analyser) analyser->LogAllMetrics(delta_time);

    // Increment simulation time
    total_simulation_time += delta_time.asSeconds();
}

void CompSimulator::ProcessInput() {

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
            spatial_boid_grid.is_visible = !spatial_boid_grid.is_visible;
            std::cout << "Visual Spatial Grid: " << spatial_boid_grid.is_visible << std::endl;
        }

        if (IsKeyPressedOnce(sf::Keyboard::Space)) {
            speed_up_sumlation = !speed_up_sumlation;
            std::cout << "Speed up Simulation: " << speed_up_sumlation << std::endl;
        }

        if (IsKeyPressedOnce(sf::Keyboard::F5)) {
            analyser->SaveBoidPerLanguageToCSV("language_output.csv");
            analyser->SavePositionPerLanguageCSV("positions_output.csv");
        }

        if (IsKeyPressedOnce(sf::Keyboard::Escape)) {
            context->state_manager->PopState();
        }
    }
    camera.Drag(mouse_pos);
};

void CompSimulator::DrawWorldAndBoids() {
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

void CompSimulator::DrawSpawners() const {
    // Set camera view
    context->window->setView(camera.view);

    // Draw Spawners
    for (const auto& spawners : boid_spawners) {
        spawners->Draw(context->window.get());
    }

    // Reset camera view to default
    context->window->setView(context->window->getDefaultView());
}

void CompSimulator::Draw() {
    context->window->clear(sf::Color::Black);
    DrawWorldAndBoids();
    context->window->display();
}

void CompSimulator::Start() {

}

void CompSimulator::Pause() {

}

void CompSimulator::AddBoid(const std::shared_ptr<CompBoid> &boid) {
    boids.push_back(boid);           // creates a copy of shared_ptr, assigning an additional owner (the 'boids' vector)
    spatial_boid_grid.AddObj(boid);
}
