//
// Created by wouter on 19-3-2024.
//

#ifndef TERRAINSETTINGSINTERFACE_H
#define TERRAINSETTINGSINTERFACE_H
#include "Panel.h"
#include "editor/Tools.h"


class TerrainToolInterface : public Panel {
public:
    TerrainToolInterface(sf::Vector2f pos, const std::shared_ptr<TerrainTool>& terrain_tool);
};



#endif //TERRAINSETTINGSINTERFACE_H