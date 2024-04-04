//
// Created by wouter on 3-4-2024.
//

#ifndef CONFIGINTERFACE_H
#define CONFIGINTERFACE_H
#include "InterfaceManager.h"
#include "components/Panel.h"
#include "SimulationData.h"

class Editor;

class ConfigInterface : public Panel {
public:

    Editor& editor;
    SimulationData& simulation_data;
    std::shared_ptr<Panel> active_config_interface;
    std::shared_ptr<Panel> key_config_interface;
    std::shared_ptr<Panel> vector_config_interface;
    std::shared_ptr<Panel> study_config_interface;

    ConfigInterface(std::shared_ptr<InterfaceManager>& interface_manager, sf::Vector2f pos, Editor& editor);

    void CreateDefaultConfigFields(SimulationData &simulation_data, SimulationConfig default_config, float fld_w,
                                   float fld_h, InterfaceManager& interface_manager);

    void SwitchTo(SimulationType type);
    void Draw(sf::RenderWindow *window) override;
};



#endif //CONFIGINTERFACE_H
