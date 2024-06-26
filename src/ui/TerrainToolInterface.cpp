//
// Created by wouter on 19-3-2024.
//

#include "TerrainToolInterface.h"

#include <iostream>

#include "components/Button.h"
#include "ResourceManager.h"
#include "components/TextField.h"
#include "editor/Tools.h"
#include "components/InputField.h"


TerrainToolInterface::TerrainToolInterface(sf::Vector2f pos, const std::shared_ptr<TerrainTool>& p_terrain_tool, InterfaceManager& interface_manager)
    : Panel(pos, sf::Vector2f(310, 130) , sf::Color(125,125,125)) {

    // input Fields
    std::vector<std::shared_ptr<InterfaceComponent>> fields;
    fields.emplace_back(std::make_shared<FloatInputField>(interface_manager, [p_terrain_tool](float value) {p_terrain_tool->friction_modifier = value;},
                                                          sf::Vector2f(0,0), sf::Vector2f(100, 30), 6, p_terrain_tool->friction_modifier));
    fields.emplace_back(std::make_shared<IntInputField>(interface_manager, [p_terrain_tool](int value) {p_terrain_tool->min_speed = value;},
                                                          sf::Vector2f(0,0), sf::Vector2f(100, 30), 6, p_terrain_tool->min_speed));
    fields.emplace_back(std::make_shared<IntInputField>(interface_manager, [p_terrain_tool](int value) {p_terrain_tool->max_speed = value;},
                                                          sf::Vector2f(0,0), sf::Vector2f(100, 30), 6, p_terrain_tool->max_speed));

    int offset = 40;
    for (int i = 0; i < fields.size(); ++i) {
        this->AddComponentWithRelativePos(fields[i], sf::Vector2f(200, 10 + offset*i));
    }

    // Text
    std::shared_ptr<InterfaceComponent> friction_modifier_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Friction Modifier", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(friction_modifier_text, sf::Vector2f(10, 10 + offset*0));

    std::shared_ptr<InterfaceComponent> min_speed_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Minimum Speed", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(min_speed_text, sf::Vector2f(10, 10 + offset*1));

    std::shared_ptr<InterfaceComponent> max_speed_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Maximum Speed", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(max_speed_text, sf::Vector2f(10, 10 + offset*2));
}

