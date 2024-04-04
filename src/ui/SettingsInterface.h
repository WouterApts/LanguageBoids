//
// Created by wouter on 21-3-2024.
//

#ifndef SETTINGSINTERFACE_H
#define SETTINGSINTERFACE_H
#include "Application.h"
#include "InterfaceManager.h"
#include "components/Panel.h"

class Editor;

class SettingsInterface : public Panel {
public:

    std::shared_ptr<Panel> escape_interface;
    std::shared_ptr<Panel> config_interface;

    std::shared_ptr<InterfaceManager> interface_manager;
    float button_size[2] = {50, 50};

    SettingsInterface(std::shared_ptr<InterfaceManager> &interface_manager, std::shared_ptr<Context> &context,
                      Editor& editor, const sf::Vector2f &pos);
    void Draw(sf::RenderWindow *window) override;
};



#endif //SETTINGSINTERFACE_H
