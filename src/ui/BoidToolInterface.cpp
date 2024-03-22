//
// Created by wouter on 20-3-2024.
//

#include "BoidToolInterface.h"

#include "ResourceManager.h"
#include "TextField.h"

BoidToolInterface::BoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool>& p_boid_tool)
    : Panel(pos, 310, 150, sf::Color(100,100,100)) {

    std::vector<std::shared_ptr<Interface>> fields;
    fields.emplace_back(std::make_shared<IntInputField>([p_boid_tool](int value) {p_boid_tool->boid_count = value;},
                                                          sf::Vector2f(0,0), 100, 30, 6, std::to_string(p_boid_tool->boid_count)));
    fields.emplace_back(std::make_shared<IntInputField>([p_boid_tool](int value) {p_boid_tool->language_key = value;},
                                                          sf::Vector2f(0,0), 100, 30, 1, std::to_string(p_boid_tool->language_key)));

    int offset = 40;
    for (int i = 0; i < fields.size(); ++i) {
        this->AddElementWithRelativePos(fields[i], sf::Vector2f(200, 10 + offset*i));
    }

    // Text
    std::shared_ptr<Interface> friction_modifier_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Boids Spawned", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddElementWithRelativePos(friction_modifier_text, sf::Vector2f(10, 10 + offset*0));

    std::shared_ptr<Interface> min_speed_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Language Key", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddElementWithRelativePos(min_speed_text, sf::Vector2f(10, 10 + offset*1));
}
