//
// Created by wouter on 18-3-2024.
//

#include "ToolSelectorInterface.h"

#include "BoidToolInterfaces.h"
#include "components/Button.h"
#include "TerrainToolInterface.h"

ToolSelectorInterface::ToolSelectorInterface(const std::shared_ptr<InterfaceManager> &interface_manager, const std::shared_ptr<ToolSelector>& p_tool_selector, sf::Vector2f pos)
    : Panel(pos, sf::Vector2f(370, 70)), p_tool_selector(p_tool_selector), interface_manager(interface_manager) {

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
    auto p_terrain_tool = std::dynamic_pointer_cast<TerrainTool>(p_tool_selector->tools[TerrainT]);
    terrain_tool_interface = std::make_shared<TerrainToolInterface>(tool_interface_position, p_terrain_tool, *this->interface_manager);
    interface_manager->AddComponent(terrain_tool_interface);
    terrain_tool_interface->Deactivate();

    auto p_keyboid_circle_tool = std::dynamic_pointer_cast<CompBoidCircleTool>(p_tool_selector->tools[KeyBoidCircleT]);
    keyboid_circle_tool_interface = std::make_shared<KeyBoidToolInterface>(tool_interface_position, p_keyboid_circle_tool, *this->interface_manager);
    interface_manager->AddComponent(keyboid_circle_tool_interface);
    keyboid_circle_tool_interface->Deactivate();

    auto p_keyboid_rectangle_tool = std::dynamic_pointer_cast<CompBoidRectangleTool>(p_tool_selector->tools[KeyBoidRectangleT]);
    keyboid_rectangle_tool_interface = std::make_shared<KeyBoidToolInterface>(tool_interface_position, p_keyboid_rectangle_tool, *this->interface_manager);
    interface_manager->AddComponent(keyboid_rectangle_tool_interface);
    keyboid_rectangle_tool_interface->Deactivate();

    auto p_vectorboid_circle_tool = std::dynamic_pointer_cast<EvoBoidCircleTool>(p_tool_selector->tools[VectorBoidCircleT]);
    vectorboid_circle_tool_interface = std::make_shared<VectorBoidToolInterface>(tool_interface_position, p_vectorboid_circle_tool, *this->interface_manager);
    interface_manager->AddComponent(vectorboid_circle_tool_interface);
    vectorboid_circle_tool_interface->Deactivate();

    auto p_vectorboid_rectangle_tool = std::dynamic_pointer_cast<EvoBoidRectangleTool>(p_tool_selector->tools[VectorBoidRectangleT]);
    vectorboid_rectangle_tool_interface = std::make_shared<VectorBoidToolInterface>(tool_interface_position, p_vectorboid_rectangle_tool, *this->interface_manager);
    interface_manager->AddComponent(vectorboid_rectangle_tool_interface);
    vectorboid_rectangle_tool_interface->Deactivate();

    auto p_study_boid_circle_tool = std::dynamic_pointer_cast<StudyBoidCircleTool>(p_tool_selector->tools[StudyBoidCircleT]);
    studyboid_circle_tool_interface = std::make_shared<StudyBoidToolInterface>(tool_interface_position, p_study_boid_circle_tool);
    interface_manager->AddComponent(studyboid_circle_tool_interface);
    studyboid_circle_tool_interface->Deactivate();

    auto p_study_boid_rectangle_tool = std::dynamic_pointer_cast<StudyBoidRectangleTool>(p_tool_selector->tools[StudyBoidRectangleT]);
    studyboid_rectangle_tool_interface = std::make_shared<StudyBoidToolInterface>(tool_interface_position, p_study_boid_rectangle_tool);
    interface_manager->AddComponent(studyboid_rectangle_tool_interface);
    studyboid_rectangle_tool_interface->Deactivate();

    // Activate the tool selector interface
    this->InterfaceComponent::Activate();
}


void ToolSelectorInterface::Draw(sf::RenderWindow* window) {
    Panel::Draw(window);

    auto selection_highlight = sf::RectangleShape(sf::Vector2f(button_size[0], button_size[1]));
    selection_highlight.setFillColor(sf::Color(255,255,255,80));

    switch (p_tool_selector->selected_tool) {
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
        case KeyBoidCircleT:
        case VectorBoidCircleT:
        case StudyBoidCircleT:
            selection_highlight.setPosition(components[4]->getPosition());
            window->draw(selection_highlight);
            break;
        case KeyBoidRectangleT:
        case VectorBoidRectangleT:
        case StudyBoidRectangleT:
            selection_highlight.setPosition(components[5]->getPosition());
            window->draw(selection_highlight);
            break;
        default:
            break;
    }
}

void ToolSelectorInterface::SelectTool(Tools tool) {
    p_tool_selector->SelectTool(tool);
}

void ToolSelectorInterface::ActivateSelectedToolInterface() {
    if (active_tool_interface) {
        active_tool_interface->Deactivate();
    }

    //Todo, attatch tool_interface to the specific tool class
    switch (p_tool_selector->selected_tool) {
        case EraserT:
            active_tool_interface = nullptr;
            break;
        case LineObstacleT:
            active_tool_interface = nullptr;
            break;
        case CircleObstacleT:
            active_tool_interface = nullptr;
            break;
        case TerrainT:
            active_tool_interface = terrain_tool_interface;
            break;
        case KeyBoidCircleT:
            active_tool_interface = keyboid_circle_tool_interface;
            break;
        case VectorBoidCircleT:
            active_tool_interface = vectorboid_circle_tool_interface;
        break;
        case StudyBoidCircleT:
            active_tool_interface = studyboid_circle_tool_interface;
            break;
        case KeyBoidRectangleT:
            active_tool_interface = keyboid_rectangle_tool_interface;
            break;
        case VectorBoidRectangleT:
            active_tool_interface = vectorboid_rectangle_tool_interface;
        break;
        case StudyBoidRectangleT:
            active_tool_interface = studyboid_rectangle_tool_interface;
            break;
        default:
            break;
    }
    // Deactivate the appropriate interface
    if (active_tool_interface) {
        active_tool_interface->Activate();
    }
}
