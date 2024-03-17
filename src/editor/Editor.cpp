//
// Created by wouter on 16-3-2024.
//

#include "Editor.h"
#include <iostream>
#include <memory>
#include <memory>
#include "ui/Button.h"
#include "ui/Panel.h"

Editor::Editor(const std::shared_ptr<Context>& context, World& world, float camera_width, float camera_height)
: context(context), camera(Camera(sf::Vector2f(world.width / 2, world.height / 2), camera_width, camera_height)), world(world) {
}

void Editor::Init() {
    // Initialize Interfaces
    Initialize_Tools_interface();
}

void Editor::ProcessInput() {


    sf::Event event{};
    context->window->setView(camera.view);
    auto mouse_pos = sf::Mouse::getPosition(*context->window);
    tool_pos = context->window->mapPixelToCoords(mouse_pos);
    if (grid_spacing > 0) {
        tool_pos = MapPixelToClosestGridCoords(mouse_pos);
    }

    while (context->window->pollEvent(event)) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if interfaces are clicked.
                bool interface_clicked = false;
                for (auto& interface : interfaces) {
                    //Change view to default to acces UI coordinates. TODO: handle everything UI related in a InterfaceManager
                    auto mouse_interface_pos = context->window->mapPixelToCoords(mouse_pos, context->window->getDefaultView());
                    std::cout << mouse_interface_pos.x << std::endl;
                    if (interface->IsPointInsideRect(mouse_interface_pos)) {
                        interface->OnClick(mouse_interface_pos);
                        interface_clicked = true;
                    }
                }
                // If no interfaces are clicked, use tool.

                if (!interface_clicked && p_selected_tool) p_selected_tool->OnLeftClick(tool_pos, &world);
            }

            if (event.mouseButton.button == sf::Mouse::Right) {
                if (p_selected_tool) p_selected_tool->OnRightClick(tool_pos, &world);
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

    for (auto& obstacle : world.obstacles) {
        obstacle->Draw(context->window.get());
    }
    if (p_selected_tool) p_selected_tool->Draw(tool_pos, context->window.get());


    context->window->setView(context->window->getDefaultView());
    for (auto& interface : interfaces) {
        interface->Draw(context->window.get());
    }

    context->window->display();
}

void Editor::SelectTool(Tools tool) {
    switch (tool) {
        case Tools::LineObstacleTool:
            p_selected_tool = std::make_unique<LineObstacleTool>();
            std::cout << "Selected the Line Obstacle Tool!" << std::endl;
            break;
        case Tools::CircleObstacleTool:
            p_selected_tool = std::make_unique<CircleObstacleTool>();
            std::cout << "Selected the Circle Obstacle Tool!" << std::endl;
            break;
        // Add other cases for different tools
        default:
            break;
    }
}


void Editor::Initialize_Tools_interface() {
    std::shared_ptr<Panel> tools_panel = std::make_shared<Panel>(sf::Vector2f(20,20), 310, 70, sf::Color(80,80,80));
    std::shared_ptr<Interface> btn_0 = std::make_shared<ImageButton>([this]() { SelectTool(Tools::LineObstacleTool); },
                                                                     "erase_tool_button", sf::Vector2f(0,0), 50, 50);
    std::shared_ptr<Interface> btn_1 = std::make_shared<ImageButton>([this]() { SelectTool(Tools::LineObstacleTool); },
                                                                     "line_tool_button", sf::Vector2f(0,0), 50, 50);
    std::shared_ptr<Interface> btn_2 = std::make_shared<ImageButton>([this]() { SelectTool(Tools::CircleObstacleTool); },
                                                                     "circle_tool_button", sf::Vector2f(0,0), 50, 50);
    std::shared_ptr<Interface> btn_3 = std::make_shared<ImageButton>([this]() { SelectTool(Tools::CircleObstacleTool); },
                                                                     "boid_circle_button", sf::Vector2f(0,0), 50, 50);
    std::shared_ptr<Interface> btn_4 = std::make_shared<ImageButton>([this]() { SelectTool(Tools::CircleObstacleTool); },
                                                                     "boid_rectangle_button", sf::Vector2f(0,0), 50, 50);
    tools_panel->AddElementWithRelativePos(btn_0, sf::Vector2f(10,10));
    tools_panel->AddElementWithRelativePos(btn_1, sf::Vector2f(70,10));
    tools_panel->AddElementWithRelativePos(btn_2, sf::Vector2f(130,10));
    tools_panel->AddElementWithRelativePos(btn_3, sf::Vector2f(190,10));
    tools_panel->AddElementWithRelativePos(btn_4, sf::Vector2f(250,10));
    interfaces.push_back(tools_panel);
}

sf::Vector2f Editor::MapPixelToClosestGridCoords(sf::Vector2i pixel_coord) const {
    auto world_pos = context->window->mapPixelToCoords(pixel_coord);
    float grid_pos_x = std::round(world_pos.x / grid_spacing);
    float grid_pos_y = std::round(world_pos.y / grid_spacing);
    return {grid_pos_x * grid_spacing, grid_pos_y * grid_spacing};
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
    sf::RectangleShape verticalLine(sf::Vector2f(2.0f*camera.zoom, world.height));
    verticalLine.setFillColor(gridColor);

    // Create a horizontal line
    sf::RectangleShape horizontalLine(sf::Vector2f(world.width, 2.0f*camera.zoom));
    horizontalLine.setFillColor(gridColor);

    // Draw vertical lines
    for (int x = 0; x <= world.width; x += grid_spacing) {
        verticalLine.setPosition(x, 0);
        window->draw(verticalLine);
    }

    // Draw horizontal lines
    for (int y = 0; y <= world.height; y += grid_spacing) {
        horizontalLine.setPosition(0, y);
        window->draw(horizontalLine);
    }
}
