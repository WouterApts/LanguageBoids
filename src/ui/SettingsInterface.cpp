//
// Created by wouter on 21-3-2024.
//

#include "SettingsInterface.h"

#include "Button.h"
#include "editor/Serialization.h"

SettingsInterface::SettingsInterface(std::vector<std::shared_ptr<Interface>>& interfaces, World& world, const sf::Vector2f &pos)
    : Panel(pos, 200, 70, sf::Color(80, 80, 80)), interfaces(interfaces) {
    std::vector<std::shared_ptr<Interface> > btns;
    btns.emplace_back(std::make_shared<ImageButton>([&world]() { serialization::SaveWorldToFile(world); },
                                                    "save_button", sf::Vector2f(0, 0), button_size[0], button_size[1]));
    btns.emplace_back(std::make_shared<ImageButton>([&world]() {},
                                                "config_button", sf::Vector2f(0, 0), button_size[0], button_size[1]));

    for (int i = 0; i < btns.size(); ++i) {
        int offset = 60;
        AddElementWithRelativePos(btns[i], sf::Vector2f(10 + offset*i,10));
    }

    this->active = true;
}
