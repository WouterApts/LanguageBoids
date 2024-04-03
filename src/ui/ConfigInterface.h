//
// Created by wouter on 3-4-2024.
//

#ifndef CONFIGINTERFACE_H
#define CONFIGINTERFACE_H
#include "InterfaceManager.h"
#include "Panel.h"
#include "SimulationData.h"


class ConfigInterface : public Panel {
public:
    SimulationData& simulation_data;
    std::shared_ptr<Panel> active_config;
    std::shared_ptr<Panel> key_config_interface;
    std::shared_ptr<Panel> vector_config_interface;
    std::shared_ptr<Panel> study_config_interface;

    void CreateDefaultConfigFields(SimulationData &simulation_data, SimulationConfig default_config, float fld_w,
                                   float fld_h, InterfaceManager& interface_manager);

    ConfigInterface(std::shared_ptr<InterfaceManager>& interface_manager, sf::Vector2f pos, SimulationData& simulation_data);

    void Draw(sf::RenderWindow *window) override;
    void SwitchToKeySimulator();
    void SwitchToVectorSimulator();
    void SwitchToDominanceStudy();
};



#endif //CONFIGINTERFACE_H
