//
// Created by wouter on 6-4-2024.
//

#include "StudyInterface.h"

#include "ResourceManager.h"

StudyInterface::StudyInterface(sf::Vector2f pos) : Panel(pos, sf::Vector2f(300,140)) {

    run_fld = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "", *ResourceManager::GetFont("arial"), sf::Color::White);
    fraction_fld = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "", *ResourceManager::GetFont("arial"), sf::Color::White);
    simulation_time_fld = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "", *ResourceManager::GetFont("arial"), sf::Color::White);

    int offset = 30;
    AddComponentWithRelativePos(fraction_fld, sf::Vector2f(10, 10));
    AddComponentWithRelativePos(run_fld, sf::Vector2f(10, 10 + offset*1));
    AddComponentWithRelativePos(simulation_time_fld, sf::Vector2f(10, 10 + offset*3));
}
