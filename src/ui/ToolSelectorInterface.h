//
// Created by wouter on 18-3-2024.
//

#ifndef TOOLSELECTORINTERFACE_H
#define TOOLSELECTORINTERFACE_H
#include "BoidToolInterfaces.h"
#include "InterfaceManager.h"
#include "components/Panel.h"
#include "editor/ToolSelector.h"
#include "ui/TerrainToolInterface.h"

class ToolSelectorInterface : public Panel {
public:

    std::shared_ptr<InterfaceManager> interface_manager;
    std::shared_ptr<TerrainToolInterface> terrain_tool_interface;

    // Key Simulator
    std::shared_ptr<KeyBoidToolInterface> keyboid_circle_tool_interface;
    std::shared_ptr<KeyBoidToolInterface> keyboid_rectangle_tool_interface;

    //Vector Simulator

    //Dominance Study
    std::shared_ptr<StudyBoidToolInterface> studyboid_circle_tool_interface;
    std::shared_ptr<StudyBoidToolInterface> studyboid_rectangle_tool_interface;

    std::shared_ptr<ToolSelector> p_tool_selector;
    std::shared_ptr<InterfaceComponent> active_tool_interface;
    float button_size[2] = {50, 50};


    ToolSelectorInterface(const std::shared_ptr<InterfaceManager> &interface_manager,
                          const std::shared_ptr<ToolSelector> &p_tool_selector,
                          sf::Vector2f pos);

    void Draw(sf::RenderWindow *window) override;
    void SelectTool(Tools tool);

    void ActivateSelectedToolInterface();
};



#endif //TOOLSELECTORINTERFACE_H
