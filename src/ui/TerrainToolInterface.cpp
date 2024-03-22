//
// Created by wouter on 19-3-2024.
//

#include "TerrainToolInterface.h"

#include <iostream>

#include "Button.h"
#include "ResourceManager.h"
#include "TextField.h"
#include "editor/Tools.h"
#include "ui/InputField.h"


TerrainToolInterface::TerrainToolInterface(sf::Vector2f pos, const std::shared_ptr<TerrainTool>& p_terrain_tool)
    : Panel(pos, 310, 150, sf::Color(100,100,100)) {

    // input Fields
    std::vector<std::shared_ptr<Interface>> fields;
    fields.emplace_back(std::make_shared<FloatInputField>([p_terrain_tool](float value) {p_terrain_tool->friction_modifier = value;},
                                                          sf::Vector2f(0,0), 100, 30, 6, std::to_string(p_terrain_tool->friction_modifier)));
    fields.emplace_back(std::make_shared<IntInputField>([p_terrain_tool](int value) {p_terrain_tool->min_speed = value;},
                                                          sf::Vector2f(0,0), 100, 30, 6, std::to_string(p_terrain_tool->min_speed)));
    fields.emplace_back(std::make_shared<IntInputField>([p_terrain_tool](int value) {p_terrain_tool->max_speed = value;},
                                                          sf::Vector2f(0,0), 100, 30, 6, std::to_string(p_terrain_tool->max_speed)));

    int offset = 40;
    for (int i = 0; i < fields.size(); ++i) {
        this->AddElementWithRelativePos(fields[i], sf::Vector2f(200, 10 + offset*i));
    }

    // Text
    std::shared_ptr<Interface> friction_modifier_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Friction Modifier", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddElementWithRelativePos(friction_modifier_text, sf::Vector2f(10, 10 + offset*0));

    std::shared_ptr<Interface> min_speed_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Minimum Speed", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddElementWithRelativePos(min_speed_text, sf::Vector2f(10, 10 + offset*1));

    std::shared_ptr<Interface> max_speed_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Maximum Speed", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddElementWithRelativePos(max_speed_text, sf::Vector2f(10, 10 + offset*2));
}

