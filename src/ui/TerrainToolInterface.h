//
// Created by wouter on 19-3-2024.
//

#ifndef TERRAINSETTINGSINTERFACE_H
#define TERRAINSETTINGSINTERFACE_H
#include "components/Panel.h"
#include "editor/Tools.h"


class TerrainToolInterface : public Panel {
public:
    TerrainToolInterface(sf::Vector2f pos, const std::shared_ptr<TerrainTool>& p_terrain_tool, InterfaceManager& interface_manager);
};



#endif //TERRAINSETTINGSINTERFACE_H
