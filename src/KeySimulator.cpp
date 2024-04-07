//
// Created by wouter on 27-2-2024.
//

#include <iostream>
#include <memory>

#include "BoidSpawners.h"
#include "LanguageManager.h"
#include "Simulator.h"
#include "Utility.h"
#include "editor/Serialization.h"

KeySimulator::KeySimulator(std::shared_ptr<Context>& context, KeySimulationData& simulation_data, float camera_width, float camera_height)
    : Simulator(context, simulation_data.config, simulation_data.world, camera_width, camera_height),
      boid_spawners(simulation_data.boid_spawners),
      spatial_boid_grid(SpatialGrid<KeyBoid>(world.size().cast<int>(), static_cast<int>(config->INTERACTION_RADIUS)))
      //analyser(world, 100, boids, language_manager)
      {

    std::map<int, int> languages;
    for (auto& spawner : boid_spawners) {
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
    float max_zoom = std::max((world.height*1.1 / camera.default_height), world.width*1.1 / camera.default_width);
    camera.SetZoom(static_cast<float>(static_cast<int>(max_zoom * 10)) / 10.f);

    // Spawn Boids
    for (auto& spawner : boid_spawners) {
        spawner->AddBoids(world, config, boids);
    }

    // Initialize boids in spatial grid
    for (auto& boid : boids) {
        spatial_boid_grid.AddObj(boid);
    }

    for (auto& boid : boids) {
        boid->UpdateColor(language_manager);
    }
};


void KeySimulator::Update(sf::Time delta_time) {

    // Log metrics based on interval settings
    //analyser.LogAllMetrics(delta_time);

    for (const auto& boid: boids) {

        //Get boids in perception radius:
        std::vector<KeyBoid*> interacting_boids = std::move(spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid));
        std::vector<KeyBoid*> perceived_boids = std::move(spatial_boid_grid.ObjRadiusSearch(boid->perception_radius, boid));

        //Update boids acceleration
        boid->UpdateAcceleration(interacting_boids, world);

        //Update boid behaviour based on terrain effects
        bool in_terrain = false;
        for (auto& terrain : world.terrains) {
            if (terrain->IsPointInside(boid->pos)) {
                terrain->ApplyEffects(boid.get());
                in_terrain = true;
            }
        }
        if (!in_terrain) boid->SetDefaultMinMaxSpeed();

        //Update boids language
        boid->UpdateLanguageSatisfaction(perceived_boids, interacting_boids, language_manager, delta_time);
    }

    for (const auto& boid : boids) {
        //Update boids velocity (Also checking Collisions)
        boid->UpdateVelocity(world.obstacles, delta_time);

        //Update boids position
        boid->UpdatePosition(delta_time);
        spatial_boid_grid.UpdateObj(boid);

        //Update boids sprite
        boid->UpdateSprite();
    }

    // Increment simulation time
    total_simulation_time += delta_time.asSeconds();
};

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
