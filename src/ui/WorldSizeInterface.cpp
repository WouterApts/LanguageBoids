//
// Created by wouter on 4-4-2024.
//

#include "WorldSizeInterface.h"

#include "components/Button.h"

WorldSizeInterface::WorldSizeInterface(sf::Vector2f pos, World &world, std::shared_ptr<InterfaceManager> & interface_manager)
    : Panel(pos, sf::Vector2f(190, 70)), world(world) {

    width_fld = std::make_shared<IntInputField>(*interface_manager, [](int value) {},
                                                    sf::Vector2f(0,0), sf::Vector2f(90, 24), 6, world.width);
    height_fld = std::make_shared<IntInputField>(*interface_manager, [](int value) {},
                                                     sf::Vector2f(0,0), sf::Vector2f(90, 24), 6, world.height);
    auto apply_btn = std::make_shared<TextButton>([&](){world.width = width_fld->value; world.height = height_fld->value;}, "WORLD\n  SIZE", sf::Vector2f(0,0), sf::Vector2f(70,50),16,
                                                 sf::Color::White, sf::Color::Transparent, sf::Color::White, 2);

    this->AddComponentWithRelativePos(width_fld, sf::Vector2f(10, 10));
    this->AddComponentWithRelativePos(height_fld, sf::Vector2f(10, 37));
    this->AddComponentWithRelativePos(apply_btn, sf::Vector2f(110, 10));

}
