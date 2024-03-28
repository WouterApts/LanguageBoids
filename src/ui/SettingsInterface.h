//
// Created by wouter on 21-3-2024.
//

#ifndef SETTINGSINTERFACE_H
#define SETTINGSINTERFACE_H
#include "Application.h"
#include "InterfaceManager.h"
#include "Panel.h"
#include "SimulationData.h"


class SettingsInterface : public Panel {
public:

    std::shared_ptr<InterfaceComponent> escape_interface;
    std::shared_ptr<InterfaceManager> interface_manager;
    float button_size[2] = {50, 50};

    SettingsInterface(const std::shared_ptr<InterfaceManager> &interface_manager, std::shared_ptr<Context> &context,
                      SimulationData& simulation_data, const sf::Vector2f &pos);
};



#endif //SETTINGSINTERFACE_H
