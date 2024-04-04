//
// Created by wouter on 19-3-2024.
//

#ifndef INTERFACEMANAGER_H
#define INTERFACEMANAGER_H
#include <memory>
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>

#include "components/InterfaceComponent.h"

class InputField;

class InterfaceManager {
public:
    std::vector<std::shared_ptr<InterfaceComponent>> components;
    InputField* focused_input_field;
    bool interface_clicked;

    void AddComponent(const std::shared_ptr<InterfaceComponent>& component);
    void RemoveComponent(const std::shared_ptr<InterfaceComponent>& component);
    void DrawComponents(sf::RenderWindow* window);
    void OnLeftClick(sf::Vector2f mouse_pos);
    void OnRightClick(sf::Vector2f mouse_pos);
    void OnMouseEnter(sf::Vector2f mouse_pos);
    void OnMouseLeave(sf::Vector2f mouse_pos);
    void OnKeyBoardEnter(sf::Uint32 unicode);
};



#endif //INTERFACEMANAGER_H
