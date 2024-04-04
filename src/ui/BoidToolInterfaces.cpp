//
// Created by wouter on 20-3-2024.
//

#include "BoidToolInterfaces.h"

#include "ResourceManager.h"
#include "components/TextField.h"

KeyBoidToolInterface::KeyBoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool>& p_boid_tool, InterfaceManager& interface_manager)
    : Panel(pos, sf::Vector2f(310, 90), sf::Color(125,125,125)) {

    std::vector<std::shared_ptr<InterfaceComponent>> fields;
    fields.emplace_back(std::make_shared<IntInputField>(interface_manager, [p_boid_tool](int value) {p_boid_tool->boid_count = value;},
                                                          sf::Vector2f(0,0), sf::Vector2f(100, 30), 6, p_boid_tool->boid_count));
    fields.emplace_back(std::make_shared<IntInputField>(interface_manager, [p_boid_tool](int value) {p_boid_tool->language_key = value;},
                                                          sf::Vector2f(0,0), sf::Vector2f(100, 30), 1, p_boid_tool->language_key));

    int offset = 40;
    for (int i = 0; i < fields.size(); ++i) {
        this->AddComponentWithRelativePos(fields[i], sf::Vector2f(200, 10.f + offset*i));
    }

    // Text
    std::shared_ptr<InterfaceComponent> friction_modifier_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Boids Spawned", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(friction_modifier_text, sf::Vector2f(10, 10.f + offset*0));

    std::shared_ptr<InterfaceComponent> min_speed_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Language Key", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(min_speed_text, sf::Vector2f(10, 10.f + offset*1));
}
