//
// Created by wouter on 20-3-2024.
//

#ifndef BOIDTOOLINTERFACE_H
#define BOIDTOOLINTERFACE_H
#include "components/Panel.h"
#include "editor/Tools.h"


class KeyBoidToolInterface : public Panel {
public:
    KeyBoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool>& p_boid_tool, InterfaceManager& interface_manager);
};

class VectorBoidToolInterface : public Panel {
public:
    VectorBoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool>& p_boid_tool, InterfaceManager& interface_manager);
};

class StudyBoidToolInterface : public Panel {
public:

    sf::RectangleShape selection_outline_rect;
    float button_size[2] = {30, 30};
    std::shared_ptr<BoidTool> p_boid_tool;

    StudyBoidToolInterface(sf::Vector2f pos, const std::shared_ptr<BoidTool> &p_boid_tool);
    void Draw(sf::RenderWindow *window) override;
};

#endif //BOIDTOOLINTERFACE_H
