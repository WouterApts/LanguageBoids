//
// Created by wouter on 20-3-2024.
//

#ifndef BOIDTOOLINTERFACE_H
#define BOIDTOOLINTERFACE_H
#include "Panel.h"
#include "editor/Tools.h"


class BoidToolInterface : public Panel {
public:
    BoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool>& p_boid_tool);
};



#endif //BOIDTOOLINTERFACE_H
