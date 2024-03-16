//
// Created by wouter on 27-2-2024.
//

#include <iostream>
#include <memory>

#include "LanguageManager.h"
#include "Simulation.h"
#include "Utility.h"

CompSimulation::CompSimulation(std::shared_ptr<Context>& context, World& world, const std::vector<float>& language_statuses, float camera_width, float camera_height)
    : Simulation(context, world, camera_width, camera_height),
      spatial_boid_grid(SpatialGrid<CompBoid>(world.size().cast<int>(), static_cast<int>(INTERACTION_RADIUS))),
      language_manager(LanguageManager(language_statuses)),
      analyser(CompAnalyser(world, 100, boids, language_manager)){

}

void CompSimulation::Init() {

    CreateWorldBorderLines();

    //Create boid clusters
    auto pos = Eigen::Vector2f(1000, 500);
    AddBoidCluster(pos, 200, 700, 1);
    pos = Eigen::Vector2f(1000, 1500);
    AddBoidCluster(pos, 200, 700, 2);
    pos = Eigen::Vector2f(3000, 500);
    AddBoidCluster(pos, 200, 700, 3);
    pos = Eigen::Vector2f(3000, 1500);
    AddBoidCluster(pos, 200, 700, 0);
    for (auto& boid : boids) {
        boid->UpdateColor(language_manager);
    }

    //Create Some Obstacles
    auto p1 = Eigen::Vector2f(1000,1000);
    auto p2 = Eigen::Vector2f(3000,1000);
    auto line = std::make_shared<LineObstacle>(p1, p2, 10, sf::Color::White);
    obstacles.push_back(line);

    // auto c1 = Eigen::Vector2f(1000, 1000);
    // obstacles.push_back(std::make_shared<CircleObstacle>(c1, 150, sf::Color::White));
    auto c2 = Eigen::Vector2f(2000, 1000);
    obstacles.push_back(std::make_shared<CircleObstacle>(c2, 250, sf::Color::White));

    // Create some terrain
    std::vector<Eigen::Vector2f> vertices;
    auto t1 = Eigen::Vector2f(4000,0); auto t2 = Eigen::Vector2f(4500,0);
    auto t3 = Eigen::Vector2f(4500,3000); auto t4 = Eigen::Vector2f(4000,3000);
    vertices.push_back(t1); vertices.push_back(t2); vertices.push_back(t3); vertices.push_back(t4);
    auto terrain = new Terrain(vertices, 1, 1, MIN_SPEED-25, MAX_SPEED-25);
    world.terrains.push_back(terrain);
};


void CompSimulation::Update(sf::Time delta_time) {

    // Check if enough time has passed between analysis logs
    passedTime += delta_time;
    auto analyse_frequency = sf::seconds(1.f);
    bool analyse = passedTime > analyse_frequency;
    if (analyse) {
        passedTime = passedTime - analyse_frequency;
    }

    for (const auto& boid: boids) {

        //Get boids in perception radius:
        std::vector<CompBoid*> interacting_boids = std::move(spatial_boid_grid.ObjRadiusSearch(boid->interaction_radius, boid));
        std::vector<CompBoid*> perceived_boids = std::move(spatial_boid_grid.ObjRadiusSearch(PERCEPTION_RADIUS, boid));

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

    // Analyse simulation data
    if (analyse) {
        analyser.LogLanguagesPerCell();
        analyser.LogBoidsPerLanguage();
        std::cout << "logging complete" << std::endl;
    }

    for (const auto& boid : boids) {
        //Update boids velocity (Also checking Collisions)
        boid->UpdateVelocity(obstacles, delta_time);

        //Update boids position
        boid->UpdatePosition(delta_time);
        spatial_boid_grid.UpdateObj(boid);

        //Update boids sprite
        boid->UpdateSprite();
    }
};

void CompSimulation::ProcessInput() {

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    sf::Event event{};
    while(context->window->pollEvent(event)) {

        if (event.type == sf::Event::Closed) {
            context->window->close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            //Boid Selection
            ProcessBoidSelection(context.get(), mouse_pos, spatial_boid_grid);
            //Camera Drag
            camera.StartDragging(mouse_pos);
        }

        if (event.type == sf::Event::MouseButtonReleased) {
            //Camera Drag
            camera.StopDragging();
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
        analyser.SaveBoidPerLanguageToCSV("bpl_output.csv");
        analyser.SaveLanguagesPerCellToCSV("lpc_output.csv");
    }

    camera.Drag(mouse_pos);
};

void CompSimulation::Draw() {
    context->window->clear(sf::Color::Black);
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
    for (const auto& obstacle : obstacles) {
        obstacle->Draw(context->window.get());
    }

    // Draw Boid Selection Circle
    DrawBoidSelectionCircle();
    context->window->display();
}

void CompSimulation::Start() {

}

void CompSimulation::Pause() {

}

void CompSimulation::AddBoid(const std::shared_ptr<CompBoid> &boid) {
    boids.push_back(boid);           // creates a copy of shared_ptr, assigning an additional owner (boids)
    spatial_boid_grid.AddObj(boid);
}

void CompSimulation::AddBoidCluster(Eigen::Vector2f position, float radius, int amount, int language_key) {
    Eigen::Vector2f zero_vector = Eigen::Vector2f::Zero();

    for (int i = 0; i < amount; ++i) {
        float angle = GetRandomFloatBetween(0, 2*std::numbers::pi);
        float length = GetRandomFloatBetween(0, radius);
        float x = position.x() + std::cos(angle) * length;
        float y = position.y() + std::sin(angle) * length;

        //Keep spawn position within world border
        x = std::max(std::min(x, world.width), 0.f);
        y = std::max(std::min(y, world.height), 0.f);

        auto spawn_pos = Eigen::Vector2f(x, y);
        AddBoid(std::make_shared<CompBoid>(spawn_pos, zero_vector, zero_vector, language_key));
    }
}
