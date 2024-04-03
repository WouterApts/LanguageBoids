//
// Created by wouter on 18-3-2024.
//

#include "ToolSelectorInterface.h"

#include "BoidToolInterface.h"
#include "Button.h"
#include "TerrainToolInterface.h"

ToolSelectorInterface::ToolSelectorInterface(std::shared_ptr<InterfaceManager> interface_manager, const std::shared_ptr<ToolSelector>& t_selector, sf::Vector2f pos)
    : Panel(pos, sf::Vector2f(370, 70)), tool_selector(t_selector), interface_manager(interface_manager) {

    // Add ToolSelectorInterface to interfaces
    std::vector<std::shared_ptr<InterfaceComponent>> btns;
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(EraserT); }, "erase_tool_button",
                                                      sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1])));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(LineObstacleT); },
                                                                    "line_tool_button", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1])));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(CircleObstacleT); },
                                                                     "circle_tool_button", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1])));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(TerrainT); },
                                                                     "terrain_button", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1])));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(BoidCircleT); },
                                                                     "boid_circle_button", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1])));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(BoidRectangleT); },
                                                                 "boid_rectangle_button", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1])));

    for (int i = 0; i < btns.size(); ++i) {
        int offset = 60;
        AddComponentWithRelativePos(btns[i], sf::Vector2f(10.f + offset*i,10));
    }

    // Add TerrainToolInterface to interfaces
    auto tool_interface_position = sf::Vector2f(pos.x, pos.y + this->rect.getSize().y + 10);
    auto p_terrain_tool = std::dynamic_pointer_cast<TerrainTool>(tool_selector->tools[TerrainT]);
    terrain_tool_interface = std::make_shared<TerrainToolInterface>(tool_interface_position, p_terrain_tool, *this->interface_manager);
    interface_manager->AddComponent(terrain_tool_interface);
    terrain_tool_interface->Deactivate();

    auto p_boid_circle_tool = std::dynamic_pointer_cast<KeyBoidCircleTool>(tool_selector->tools[BoidCircleT]);
    boid_circle_tool_interface = std::make_shared<BoidToolInterface>(tool_interface_position, p_boid_circle_tool, *this->interface_manager);
    interface_manager->AddComponent(boid_circle_tool_interface);
    boid_circle_tool_interface->Deactivate();

    auto p_boid_rectangle_tool = std::dynamic_pointer_cast<KeyBoidRectangleTool>(tool_selector->tools[BoidRectangleT]);
    boid_rectangle_tool_interface = std::make_shared<BoidToolInterface>(tool_interface_position, p_boid_rectangle_tool, *this->interface_manager);
    interface_manager->AddComponent(boid_rectangle_tool_interface);
    boid_rectangle_tool_interface->Deactivate();

    // Activate the tool selector interface
    this->Activate();
}


void ToolSelectorInterface::Draw(sf::RenderWindow* window) {
    Panel::Draw(window);

    auto selection_highlight = sf::RectangleShape(sf::Vector2f(static_cast<float>(button_size[0]), static_cast<float>(button_size[1])));
    selection_highlight.setFillColor(sf::Color(255,255,255,100));

    switch (tool_selector->selected_tool) {
        case EraserT:
            selection_highlight.setPosition(components[0]->getPosition());
            window->draw(selection_highlight);
            break;
        case LineObstacleT:
            selection_highlight.setPosition(components[1]->getPosition());
            window->draw(selection_highlight);
            break;
        case CircleObstacleT:
            selection_highlight.setPosition(components[2]->getPosition());
            window->draw(selection_highlight);
            break;
        case TerrainT:
            selection_highlight.setPosition(components[3]->getPosition());
            window->draw(selection_highlight);
            break;
        case BoidCircleT:
            selection_highlight.setPosition(components[4]->getPosition());
            window->draw(selection_highlight);
            break;
        case BoidRectangleT:
            selection_highlight.setPosition(components[5]->getPosition());
            window->draw(selection_highlight);
            break;
        default:
            break;
    }
}

void ToolSelectorInterface::SelectTool(Tools tool) {
    // Deactivate the current interface
    if (active_tool_interface) {
        active_tool_interface->Deactivate();
    }

    switch (tool) {
        case EraserT:
            tool_selector->SelectTool(EraserT);
            active_tool_interface = nullptr;
            break;
        case LineObstacleT:
            tool_selector->SelectTool(LineObstacleT);
            active_tool_interface = nullptr;
            break;
        case CircleObstacleT:
            tool_selector->SelectTool(CircleObstacleT);
            active_tool_interface = nullptr;
            break;
        case TerrainT:
            tool_selector->SelectTool(TerrainT);
            active_tool_interface = terrain_tool_interface;
            break;
        case BoidCircleT:
            tool_selector->SelectTool(BoidCircleT);
            active_tool_interface = boid_circle_tool_interface;
            break;
        case BoidRectangleT:
            tool_selector->SelectTool(BoidRectangleT);
            active_tool_interface = boid_rectangle_tool_interface;
            break;
        default: ;
    }

    // Deactivate the appropriate interface
    if (active_tool_interface) {
        active_tool_interface->Activate();
    }
}
