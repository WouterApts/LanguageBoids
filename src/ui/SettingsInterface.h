//
// Created by wouter on 21-3-2024.
//

#ifndef SETTINGSINTERFACE_H
#define SETTINGSINTERFACE_H
#include "Panel.h"
#include "World.h"


class SettingsInterface : public Panel {
public:
    SettingsInterface(std::vector<std::shared_ptr<Interface>>& interfaces, World& world, const sf::Vector2f &pos);

    std::vector<std::shared_ptr<Interface>>& interfaces;
    int button_size[2] = {50, 50};
};



#endif //SETTINGSINTERFACE_H
