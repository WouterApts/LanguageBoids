//
// Created by wouter on 18-3-2024.
//

#ifndef TOOLSELECTORINTERFACE_H
#define TOOLSELECTORINTERFACE_H
#include "BoidToolInterface.h"
#include "InterfaceManager.h"
#include "Panel.h"
#include "editor/ToolSelector.h"
#include "ui/TerrainToolInterface.h"

class ToolSelectorInterface : public Panel {
public:

    std::shared_ptr<InterfaceManager> interface_manager;

    std::shared_ptr<TerrainToolInterface> terrain_tool_interface;
    std::shared_ptr<BoidToolInterface> boid_circle_tool_interface;
    std::shared_ptr<BoidToolInterface> boid_rectangle_tool_interface;
    std::shared_ptr<ToolSelector> tool_selector;
    std::shared_ptr<InterfaceComponent> active_tool_interface;
    float button_size[2] = {50, 50};


    ToolSelectorInterface(std::shared_ptr<InterfaceManager> interface_manager,
                          const std::shared_ptr<ToolSelector> &t_selector,
                          sf::Vector2f pos);

    void Draw(sf::RenderWindow *window) override;
    void SelectTool(Tools tool);

};



#endif //TOOLSELECTORINTERFACE_H
