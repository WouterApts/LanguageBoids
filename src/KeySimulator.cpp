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
#include "editor/Serialization.h"

KeySimulator::KeySimulator(std::shared_ptr<Context>& context, KeySimulationData& simulation_data, float camera_width, float camera_height)
    : Simulator(context, simulation_data.config, simulation_data.world, camera_width, camera_height),
      boid_spawners(simulation_data.boid_spawners),
      num_threads(std::thread::hardware_concurrency()),
      spatial_boid_grid(SpatialGrid<KeyBoid>(world.size().cast<int>(), static_cast<int>(config->INTERACTION_RADIUS)))
     //analyser(world, 100, boids, language_manager)
{
    std::map<int, int> languages;
    for (auto &spawner: boid_spawners) {
        languages[spawner->language_key] += 1;
    }
    int number_of_languages = static_cast<int>(languages.size());
    language_manager = LanguageManager(number_of_languages);
    //analyser = KeyAnalyser(world, 100, boids, language_manager);

}

void KeySimulator::Init() {
    // Setup analysis logging
    //analyser.SetBPLTimeInterval(sf::seconds(1.f));
    //analyser.SetLPCTimeInterval(sf::seconds(5.f));

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
    }

    // Divide boids into chunks
    if (config->MULTI_THREADING) {
        DivideBoidChunks();
    }
}

void KeySimulator::DivideBoidChunks() {
    // Split the boids vector into chunks for each thread
    boid_chunks.resize(num_threads);
    for (size_t i = 0; i < boids.size(); ++i) {
        boid_chunks[i % num_threads].push_back(boids[i]);
    }
}

KeySimulator::UpdatedBoidValues KeySimulator::UpdateBoidsStepOneMultithread(const std::vector<std::shared_ptr<KeyBoid> > &boids, sf::Time delta_time) const {

    UpdatedBoidValues updated_boid_values;
    for (const auto& boid : boids) {
        //Get boids in perception radius:
        std::vector<KeyBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid);
        std::vector<KeyBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boid->perception_radius, boid);

        //Update boids acceleration
        updated_boid_values.acceleration_values[boid.get()] = boid->GetUpdatedAcceleration(interacting_boids);

        //Update boids language satisfaction
        updated_boid_values.language_and_satisfaction_values[boid.get()] = boid->GetUpdatedLanguageAndSatisfaction(perceived_boids,
                                                                                                                   interacting_boids,
                                                                                                                   delta_time);
    }
    return updated_boid_values;
}

void KeySimulator::UpdateBoidsStepOne(const std::vector<std::shared_ptr<KeyBoid>>& boids, sf::Time delta_time) const {
    for (const auto& boid : boids) {

        //Get boids in perception radius:
        std::vector<KeyBoid*> interacting_boids = spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid);
        std::vector<KeyBoid*> perceived_boids = spatial_boid_grid.ObjRadiusSearch(boid->perception_radius, boid);

        //Update boids acceleration
        boid->UpdateAcceleration(interacting_boids);

        //Update boids language satisfaction
        boid->UpdateLanguageSatisfaction(perceived_boids, interacting_boids, delta_time);
    }
}

void KeySimulator::UpdateBoidsStepTwo(const std::vector<std::shared_ptr<KeyBoid>>& boids, sf::Time delta_time) {
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

        //Update boids language
        //TODO Split MultiThread and SingleThread updates!
        boid->UpdateLanguage();

        //Update boids sprite
        boid->UpdateSprite();
    }
}

// Log metrics based on interval settings
//analyser.LogAllMetrics(delta_time);
void KeySimulator::Update(sf::Time delta_time) {

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

    // Increment simulation time
    total_simulation_time += delta_time.asSeconds();
}

void KeySimulator::ProcessInput() {

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
            //analyser.SaveBoidPerLanguageToCSV("bpl_output.csv");
            //analyser.SaveLanguagesPerCellToCSV("lpc_output.csv");
        }

        if (IsKeyPressedOnce(sf::Keyboard::Escape)) {
            context->state_manager->PopState();
        }
    }
    camera.Drag(mouse_pos);
};

void KeySimulator::DrawWorldAndBoids() {
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

void KeySimulator::DrawSpawners() const {
    // Set camera view
    context->window->setView(camera.view);

    // Draw Spawners
    for (const auto& spawners : boid_spawners) {
        spawners->Draw(context->window.get());
    }

    // Reset camera view to default
    context->window->setView(context->window->getDefaultView());
}

void KeySimulator::Draw() {
    context->window->clear(sf::Color::Black);
    DrawWorldAndBoids();
    context->window->display();
}

void KeySimulator::Start() {

}

void KeySimulator::Pause() {

}

void KeySimulator::AddBoid(const std::shared_ptr<KeyBoid> &boid) {
    boids.push_back(boid);           // creates a copy of shared_ptr, assigning an additional owner (the 'boids' vector)
    spatial_boid_grid.AddObj(boid);
}
