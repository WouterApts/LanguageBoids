//
// Created by wouter on 20-2-2024.
//

#include <SFML/Window/Event.hpp>
#include "Simulator.h"
#include "ResourceManager.h"

Simulator::Simulator(std::shared_ptr<Context>& context, std::shared_ptr<SimulationConfig>& config, World& world, float camera_width, float camera_height)
    : context(context),
      config(config),
      world(world),
      camera(Camera(sf::Vector2f(world.width / 2, world.height / 2), camera_width, camera_height)),
      selected_boid(nullptr) {

    const auto& p_texture = ResourceManager::GetTexture("boid_selection");
    boid_selection_border.setTexture(*p_texture);
    boid_selection_border.setOrigin(p_texture->getSize().x/2.0f, p_texture->getSize().y/2.0f);
}

template <typename BoidType>
void Simulator::ProcessBoidSelection(const Context* context, sf::Vector2i& mouse_pos, SpatialGrid<BoidType>& spatial_boid_grid) {
    //Get World coordinates
    auto sf_world_pos = context->window->mapPixelToCoords(mouse_pos, camera.view);
    auto world_pos = Eigen::Vector2f(sf_world_pos.x, sf_world_pos.y);


    //Boid Selection
    auto selected_boids = spatial_boid_grid.LocalSearch(world_pos);
    for (auto& boid : selected_boids) {
        if ((boid->pos - world_pos).norm() <= boid->collision_radius) {
            if (boid == selected_boid) selected_boid = nullptr;
            else selected_boid = boid;
            break;
        }
    }
}

void Simulator::ProcessCameraZoom(const sf::Event &event) {
    //Zoom in and out
    if (event.mouseWheelScroll.delta > 0) {
        camera.Zoom(-0.2); // Zoom in
    } else if (event.mouseWheelScroll.delta < 0) {
        camera.Zoom(0.2); // Zoom out
    }
}

void Simulator::DrawBoidSelectionCircle() {
    if (selected_boid != nullptr) {

        auto DrawSelectionCircle = [this](float radius, sf::Color color) {
            sf::CircleShape circle(radius);
            circle.setPosition(selected_boid->pos.x(), selected_boid->pos.y());
            circle.setOrigin(radius, radius);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineThickness(4);
            circle.setOutlineColor(color);
            context->window->draw(circle);
        };

        // Draw selection Circle
        DrawSelectionCircle(selected_boid->interaction_radius, sf::Color(180,180,180));
        DrawSelectionCircle(selected_boid->perception_radius, sf::Color(120,120,120));

        // Draw triangular selection border
        boid_selection_border.setPosition(selected_boid->pos.x(), selected_boid->pos.y());
        auto angle = static_cast<float>(std::atan2(selected_boid->vel.y(), selected_boid->vel.x()) * 180 / std::numbers::pi);
        boid_selection_border.setRotation(angle);
        context->window->draw(boid_selection_border);
    }
}

void Simulator::CreateWorldBorderLines() {
    auto p1 = Eigen::Vector2f(0,0);
    auto p2 = Eigen::Vector2f(world.width,0);
    auto p3 = Eigen::Vector2f(world.width,world.height);
    auto p4 = Eigen::Vector2f(0,world.height);
    auto line = std::make_shared<LineObstacle>(p1, p2, 10, sf::Color::White);
    world.obstacles.push_back(line);
    line = std::make_shared<LineObstacle>(p2, p3, 10, sf::Color::White);
    world.obstacles.push_back(line);
    line = std::make_shared<LineObstacle>(p3, p4, 10, sf::Color::White);
    world.obstacles.push_back(line);
    line = std::make_shared<LineObstacle>(p4, p1, 10, sf::Color::White);
    world.obstacles.push_back(line);
}

// This explicit instantiation informs the compiler to generate the code for the template function for the below specified template arguments, making it available for linking.
template void Simulator::ProcessBoidSelection<KeyBoid>(const Context*, sf::Vector2i&, SpatialGrid<KeyBoid>&);
template void Simulator::ProcessBoidSelection<VectorBoid>(const Context*, sf::Vector2i&, SpatialGrid<VectorBoid>&);

