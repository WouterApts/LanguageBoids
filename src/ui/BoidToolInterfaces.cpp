//
// Created by wouter on 20-3-2024.
//

#include "BoidToolInterfaces.h"

#include "ResourceManager.h"
#include "components/Button.h"
#include "components/TextField.h"

KeyBoidToolInterface::KeyBoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool>& p_boid_tool, InterfaceManager& interface_manager)
    : Panel(pos, sf::Vector2f(310, 90), sf::Color(125,125,125)) {

    // Input fields
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
    std::shared_ptr<InterfaceComponent> boids_spawned_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Boids Spawned", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(boids_spawned_text, sf::Vector2f(10, 10.f + offset*0));

    std::shared_ptr<InterfaceComponent> key_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Language Key", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(key_text, sf::Vector2f(10, 10.f + offset*1));
}

VectorBoidToolInterface::VectorBoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool>& p_boid_tool, InterfaceManager& interface_manager)
    : Panel(pos, sf::Vector2f(310, 125), sf::Color(125,125,125)) {

    // Input fields
    std::vector<std::shared_ptr<InterfaceComponent>> fields;
    fields.emplace_back(std::make_shared<IntInputField>(interface_manager, [p_boid_tool](int value) {p_boid_tool->boid_count = value;},
                                                          sf::Vector2f(0,0), sf::Vector2f(100, 30), 6, p_boid_tool->boid_count));
    fields.emplace_back(std::make_shared<IntInputField>(interface_manager, [p_boid_tool](int value) {p_boid_tool->language_seed = value;},
                                                          sf::Vector2f(0,0), sf::Vector2f(100, 30), 8, p_boid_tool->language_seed));
    fields.emplace_back(std::make_shared<FloatInputField>(interface_manager, [p_boid_tool](float value) {p_boid_tool->feature_bias = value;},
                                                      sf::Vector2f(0,0), sf::Vector2f(100, 30), 4, p_boid_tool->feature_bias));

    int offset = 40;
    for (int i = 0; i < fields.size(); ++i) {
        this->AddComponentWithRelativePos(fields[i], sf::Vector2f(200, 10.f + offset*i));
    }

    // Text
    std::shared_ptr<InterfaceComponent> boids_spawned_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Boids Spawned", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(boids_spawned_text, sf::Vector2f(10, 10.f + offset*0));

    std::shared_ptr<InterfaceComponent> seed_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Language Seed", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(seed_text, sf::Vector2f(10, 10.f + offset*1));

    std::shared_ptr<InterfaceComponent> bias_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Feature Bias", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(bias_text, sf::Vector2f(10, 10.f + offset*2));
}

StudyBoidToolInterface::StudyBoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool>& p_boid_tool)
    : Panel(pos, sf::Vector2f(310, 50), sf::Color(125,125,125)), p_boid_tool(p_boid_tool) {

    // Language Selection Buttons
    auto red_btn = std::make_shared<TextButton>([p_boid_tool](){p_boid_tool->language_key = 0;}, " ", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1]),
                                                  0, sf::Color::Transparent, sf::Color::Red, sf::Color::Transparent);
    auto green_btn = std::make_shared<TextButton>([p_boid_tool](){p_boid_tool->language_key = 1;}, " ", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1]),
                                                  0, sf::Color::Transparent, sf::Color::Green, sf::Color::Transparent);

    this->AddComponentWithRelativePos(red_btn, sf::Vector2f(rect.getSize().x - 2*(button_size[0] + 10), 10));
    this->AddComponentWithRelativePos(green_btn, sf::Vector2f(rect.getSize().x - (button_size[0] + 10), 10));

    // Text
    std::shared_ptr<InterfaceComponent> language_key_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Language Key", *ResourceManager::GetFont("arial"), sf::Color::Black);
    this->AddComponentWithRelativePos(language_key_text, sf::Vector2f(10, 10.f));

    // Initialize selection outline rectangle
    selection_outline_rect.setSize(sf::Vector2f(button_size[0], button_size[1]));
    selection_outline_rect.setFillColor(sf::Color::Transparent);
    selection_outline_rect.setOutlineThickness(3);
}

void StudyBoidToolInterface::Draw(sf::RenderWindow *window) {
    Panel::Draw(window);
    if (p_boid_tool->language_key == 0) {
        selection_outline_rect.setPosition(components[0]->getPosition());
    }
    else if (p_boid_tool->language_key == 1) {
        selection_outline_rect.setPosition(components[1]->getPosition());
    }
    window->draw(selection_outline_rect);
}
