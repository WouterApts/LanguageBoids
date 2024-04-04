//
// Created by wouter on 17-3-2024.
//

#ifndef PANEL_H
#define PANEL_H
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>

#include "InputField.h"
#include "InterfaceComponent.h"


class Panel : public RectangleComponent {
public:
    std::vector<std::shared_ptr<InterfaceComponent>> components;
    std::vector<sf::Vector2f> relative_position;

    Panel(sf::Vector2f pos, sf::Vector2f size,
        sf::Color background_color = sf::Color(100,100,100),
        sf::Color outline_color = sf::Color::Transparent,
        float outline_thickness = 0.f);

    void Draw(sf::RenderWindow* window) override;
    void OnLeftClick(sf::Vector2f mouse_pos) override;
    void OnMouseEnter(sf::Vector2f mouse_pos) override;
    void OnMouseLeave(sf::Vector2f mouse_pos) override;
    void OnKeyBoardEnter(sf::Uint32 unicode) override;

    void AddComponentWithRelativePos(const std::shared_ptr<InterfaceComponent> &component, sf::Vector2f relative_pos);
    void SetPosition(const sf::Vector2f &pos) override;
};


#endif //PANEL_H
