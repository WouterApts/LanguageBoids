//
// Created by wouter on 18-3-2024.
//

#include "ToolSelectorInterface.h"

#include "BoidToolInterface.h"
#include "Button.h"
#include "TerrainToolInterface.h"

ToolSelectorInterface::ToolSelectorInterface(std::vector<std::shared_ptr<Interface>>& interfaces, const std::shared_ptr<ToolSelector>& t_selector, sf::Vector2f pos)
    : Panel(pos, 370, 70, sf::Color(80,80,80)), tool_selector(t_selector), interfaces(interfaces) {

    // Add ToolSelectorInterface to interfaces
    std::vector<std::shared_ptr<Interface>> btns;
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(EraserT); },
                                                                     "erase_tool_button", sf::Vector2f(0,0), button_size[0], button_size[1]));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(LineObstacleT); },
                                                                    "line_tool_button", sf::Vector2f(0,0), button_size[0], button_size[1]));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(CircleObstacleT); },
                                                                     "circle_tool_button", sf::Vector2f(0,0), button_size[0], button_size[1]));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(TerrainT); },
                                                                     "terrain_button", sf::Vector2f(0,0), button_size[0], button_size[1]));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(BoidCircleT); },
                                                                     "boid_circle_button", sf::Vector2f(0,0), button_size[0], button_size[1]));
    btns.emplace_back(std::make_shared<ImageButton>([this]() { this->SelectTool(BoidRectangleT); },
                                                                 "boid_rectangle_button", sf::Vector2f(0,0), button_size[0], button_size[1]));

    for (int i = 0; i < btns.size(); ++i) {
        int offset = 60;
        AddElementWithRelativePos(btns[i], sf::Vector2f(10 + offset*i,10));
    }

    // Add TerrainToolInterface to interfaces
    auto tool_interface_position = sf::Vector2f(pos.x, pos.y + this->height + 10);
    auto p_terrain_tool = std::dynamic_pointer_cast<TerrainTool>(tool_selector->tools[TerrainT]);
    terrain_tool_interface = std::make_shared<TerrainToolInterface>(tool_interface_position, p_terrain_tool);
    interfaces.push_back(terrain_tool_interface);

    auto p_boid_circle_tool = std::dynamic_pointer_cast<BoidCircleTool>(tool_selector->tools[BoidCircleT]);
    boid_circle_tool_interface = std::make_shared<BoidToolInterface>(tool_interface_position, p_boid_circle_tool);
    interfaces.push_back(boid_circle_tool_interface);

    // Activate the tool selector interface
    this->active = true;
}


void ToolSelectorInterface::Draw(sf::RenderWindow* window) {
    Panel::Draw(window);

    auto selection_highlight = sf::RectangleShape(sf::Vector2f(static_cast<float>(button_size[0]), static_cast<float>(button_size[1])));
    selection_highlight.setFillColor(sf::Color(255,255,255,100));

    switch (tool_selector->selected_tool) {
        case EraserT:
            selection_highlight.setPosition(elements[0]->pos);
            window->draw(selection_highlight);
            break;
        case LineObstacleT:
            selection_highlight.setPosition(elements[1]->pos);
            window->draw(selection_highlight);
            break;
        case CircleObstacleT:
            selection_highlight.setPosition(elements[2]->pos);
            window->draw(selection_highlight);
            break;
        case TerrainT:
            selection_highlight.setPosition(elements[3]->pos);
            window->draw(selection_highlight);
            break;
        case BoidCircleT:
            selection_highlight.setPosition(elements[4]->pos);
            window->draw(selection_highlight);
            break;
        case BoidRectangleT:
            selection_highlight.setPosition(elements[5]->pos);
            window->draw(selection_highlight);
            break;
        default:
            break;
    }
}

void ToolSelectorInterface::SelectTool(Tools tool) {
    // Deactivate the current interface
    if (active_tool_interface) {
        active_tool_interface->active = false;
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
        default: ;
    }

    // Deactivate the appropriate interface
    if (active_tool_interface) {
        active_tool_interface->active = true;
    }
}
