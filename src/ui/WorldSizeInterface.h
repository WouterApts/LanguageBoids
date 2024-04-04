//
// Created by wouter on 4-4-2024.
//

#ifndef WORLDSIZEINTERFACE_H
#define WORLDSIZEINTERFACE_H
#include "World.h"
#include "components/Panel.h"


class WorldSizeInterface : public Panel {
public:
    WorldSizeInterface(sf::Vector2f pos, World &world, std::shared_ptr<InterfaceManager>& interface_manager);

    World& world;
    std::shared_ptr<IntInputField> width_fld;
    std::shared_ptr<IntInputField> height_fld;
};



#endif //WORLDSIZEINTERFACE_H
