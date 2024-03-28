//
// Created by wouter on 21-3-2024.
//

#include "SettingsInterface.h"

#include <iostream>

#include "Application.h"
#include "Button.h"
#include "EscapeInterface.h"
#include "editor/Serialization.h"

SettingsInterface::SettingsInterface(const std::shared_ptr<InterfaceManager>& interface_manager, std::shared_ptr<Context>& context, SimulationData& simulation_data, const sf::Vector2f &pos)
    : Panel(pos, sf::Vector2f(190, 70), sf::Color(80, 80, 80)), interface_manager(interface_manager) {
    std::vector<std::shared_ptr<InterfaceComponent> > btns;
    btns.emplace_back(std::make_shared<ImageButton>([&simulation_data]() { serialization::SaveSimulationDataToFile(simulation_data); },
                                                    "save_button", sf::Vector2f(0, 0), sf::Vector2f(button_size[0], button_size[1])));
    btns.emplace_back(std::make_shared<ImageButton>([]() {},
                                                "config_button", sf::Vector2f(0, 0), sf::Vector2f(button_size[0], button_size[1])));
    btns.emplace_back(std::make_shared<ImageButton>([&]() { this->escape_interface->active = true; },
                                            "escape_button", sf::Vector2f(0, 0), sf::Vector2f(button_size[0], button_size[1])));

    for (int i = 0; i < btns.size(); ++i) {
        int offset = 60;
        AddComponentWithRelativePos(btns[i], sf::Vector2f(10.f + offset*i,10));
    }

    auto escape_interface_pos = sf::Vector2f(context->window->getSize().x/2 - 200, context->window->getSize().y/2 - 100);
    escape_interface = std::make_shared<EscapeInterface>(escape_interface_pos, context);
    interface_manager->AddComponent(escape_interface);
    this->active = true;
}
