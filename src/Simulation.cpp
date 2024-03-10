//
// Created by wouter on 20-2-2024.
//


#include <SFML/Window/Event.hpp>
#include "Simulation.h"


Simulation::Simulation(std::shared_ptr<Context>& context, World& world, float camera_width, float camera_height)
    : context(context),
      world(world),
      camera(Camera(sf::Vector2f(world.width / 2, world.height / 2), camera_width, camera_height)),
      selected_boid(nullptr) {
}

template <typename BoidType>
void Simulation::ProcessBoidSelection(const Context* context, sf::Vector2i& mouse_pos, SpatialGrid<BoidType>& spatial_boid_grid) {
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

void Simulation::ProcessCameraZoom(const sf::Event &event) {
    //Zoom in and out
    if (event.mouseWheelScroll.delta > 0) {
        camera.Zoom(-0.1); // Zoom in
    } else if (event.mouseWheelScroll.delta < 0) {
        camera.Zoom(0.1); // Zoom out
    }
}

void Simulation::DrawBoidSelectionCircle() const {
    if (selected_boid != nullptr) {
        float r = 20;
        auto selection_circle = sf::CircleShape(20);
        selection_circle.setPosition(selected_boid->pos.x(), selected_boid->pos.y());
        selection_circle.setOrigin(r, r);
        selection_circle.setFillColor(sf::Color::Transparent);
        selection_circle.setOutlineThickness(5);
        context->window->draw(selection_circle);
    }
}

void Simulation::DrawBorderOutline() const {
    auto border_rect = sf::RectangleShape(sf::Vector2f(world.width, world.height));
    border_rect.setFillColor(sf::Color::Transparent);
    border_rect.setOutlineColor(sf::Color::White);
    border_rect.setOutlineThickness(10);
    context->window->draw(border_rect);
    context->window->display();
}


// This explicit instantiation informs the compiler to generate the code for the template function for the below specified template arguments, making it available for linking.
template void Simulation::ProcessBoidSelection<CompBoid>(const Context*, sf::Vector2i&, SpatialGrid<CompBoid>&);
template void Simulation::ProcessBoidSelection<EvoBoid>(const Context*, sf::Vector2i&, SpatialGrid<EvoBoid>&);

