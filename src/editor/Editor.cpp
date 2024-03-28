//
// Created by wouter on 16-3-2024.
//

#include "Editor.h"
#include <iostream>
#include <memory>

#include "BoidSpawners.h"
#include "Serialization.h"
#include "ToolSelector.h"
#include "Utility.h"
#include "ui/Button.h"
#include "ui/Panel.h"
#include "ui/SettingsInterface.h"
#include "ui/ToolSelectorInterface.h"

Editor::Editor(const std::shared_ptr<Context>& context, const SimulationData& simulation_data, float camera_width, float camera_height)
    : context(context),
      camera(Camera(sf::Vector2f(simulation_data.world.width / 2, simulation_data.world.height / 2), camera_width, camera_height)),
      simulation_data(simulation_data),
      interface_manager(std::make_shared<InterfaceManager>()) {
}

void Editor::Init() {

    // Fit Camera view to world
    camera.SetZoom(static_cast<float>(static_cast<int>(simulation_data.world.width*1.1 / camera.default_width * 10)) / 10.f);

    // Initialize ToolSelector and Tool Interfaces
    tool_selector = std::make_shared<ToolSelector>();
    auto tool_selector_interface = std::make_shared<ToolSelectorInterface>(interface_manager, tool_selector, sf::Vector2f(20, 20));
    interface_manager->AddComponent(tool_selector_interface);

    // Initialize Settings Interfacer
    auto settings_interface_pos = sf::Vector2f(context->window->getSize().x - 220, 20);
    auto settings_interface = std::make_shared<SettingsInterface>(interface_manager, context, simulation_data, settings_interface_pos);
    interface_manager->AddComponent(settings_interface);
}

void Editor::ProcessInput() {

    sf::Event event{};
    context->window->setView(camera.view);
    auto mouse_pos = sf::Mouse::getPosition(*context->window);
    tool_pos = context->window->mapPixelToCoords(mouse_pos);
    if (grid_spacing > 0) {
        tool_pos = MapPixelToClosestGridCoords(mouse_pos);
    }

    auto mouse_interface_pos = context->window->mapPixelToCoords(mouse_pos, context->window->getDefaultView());
    interface_manager->OnMouseEnter(mouse_interface_pos);
    interface_manager->OnMouseLeave(mouse_interface_pos);

    while (context->window->pollEvent(event)) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if interfaces are clicked.
                interface_manager->interface_clicked = false;
                interface_manager->OnLeftClick(mouse_interface_pos);
                if (!interface_manager->interface_clicked && tool_selector->GetSelectedTool()) {
                    tool_selector->GetSelectedTool()->OnLeftClick(tool_pos, simulation_data);
                }
            }

            if (event.mouseButton.button == sf::Mouse::Right) {
                if (tool_selector->GetSelectedTool()) tool_selector->GetSelectedTool()->OnRightClick(tool_pos, &simulation_data.world);
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
            if (event.mouseWheelScroll.delta > 0) {
                camera.Zoom(-0.2); // Zoom in
            } else if (event.mouseWheelScroll.delta < 0) {
                camera.Zoom(0.2); // Zoom out
            }
        }

        if (event.type == sf::Event::TextEntered) {
                interface_manager->OnKeyBoardEnter(event.text.unicode);
        }

        if (IsKeyPressedOnce(sf::Keyboard::G)) {
            NextGridSize();
        }

        if (IsKeyPressedOnce(sf::Keyboard::F5)) {
            serialization::SaveSimulationDataToFile(simulation_data);
        }

        if (IsKeyPressedOnce(sf::Keyboard::Escape)) {
            context->state_manager->PopState();
        }
    }

    camera.Drag(mouse_pos);

}

void Editor::Update(sf::Time delta_time) {
    //Draw current editor state
}

void Editor::Pause() {
}

void Editor::Start() {
}

void Editor::Draw() {
    context->window->clear(sf::Color::Black);
    context->window->setView(camera.view);

    if (grid_spacing > 0) {
        DrawCursorHighlightOnGrid();
        DrawGrid(context->window.get());
    }

    for (auto& terrain : simulation_data.world.terrains) {
        terrain->Draw(context->window.get());
    }
    for (auto& spawner : simulation_data.boid_spawners) {
        spawner->Draw(context->window.get());
    }
    for (auto& obstacle : simulation_data.world.obstacles) {
        obstacle->Draw(context->window.get());
    }

    if (tool_selector->GetSelectedTool()) tool_selector->GetSelectedTool()->Draw(tool_pos, context->window.get());

    context->window->setView(context->window->getDefaultView());
    interface_manager->DrawComponents(context->window.get());

    context->window->display();
}

sf::Vector2f Editor::MapPixelToClosestGridCoords(sf::Vector2i pixel_coord) const {
    auto world_pos = context->window->mapPixelToCoords(pixel_coord);
    float grid_pos_x = std::round(world_pos.x / static_cast<float>(grid_spacing));
    float grid_pos_y = std::round(world_pos.y / static_cast<float>(grid_spacing));
    return {grid_pos_x * static_cast<float>(grid_spacing), grid_pos_y * static_cast<float>(grid_spacing)};
}

void Editor::DrawCursorHighlightOnGrid() const {
    float radius = 20;
    auto highlight = sf::CircleShape(radius);
    highlight.setOrigin(radius, radius);
    highlight.setPosition(tool_pos);
    highlight.setFillColor(sf::Color(255,255,255,100));
    context->window->draw(highlight);
}


void Editor::DrawGrid(sf::RenderWindow* window) const {
    // Define the color of the grid lines (white)
    sf::Color gridColor = sf::Color(25,25,25);

    // Create a vertical line
    sf::RectangleShape verticalLine(sf::Vector2f(2.0f*camera.zoom, simulation_data.world.height));
    verticalLine.setFillColor(gridColor);

    // Create a horizontal line
    sf::RectangleShape horizontalLine(sf::Vector2f(simulation_data.world.width, 2.0f*camera.zoom));
    horizontalLine.setFillColor(gridColor);

    // Draw vertical lines
    for (int x = 0; x <= simulation_data.world.width; x += grid_spacing) {
        verticalLine.setPosition(x, 0);
        window->draw(verticalLine);
    }

    // Draw horizontal lines
    for (int y = 0; y <= simulation_data.world.height; y += grid_spacing) {
        horizontalLine.setPosition(0, y);
        window->draw(horizontalLine);
    }
}

void Editor::NextGridSize() {
    selected_grid_spacing = (selected_grid_spacing + 1) % static_cast<int>(grid_spacings.size());
    grid_spacing = grid_spacings[selected_grid_spacing];
}
