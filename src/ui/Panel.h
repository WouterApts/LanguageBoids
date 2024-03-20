//
// Created by wouter on 17-3-2024.
//

#ifndef PANEL_H
#define PANEL_H
#include <memory>
#include <SFML/Graphics/RectangleShape.hpp>

#include "InputField.h"
#include "Interface.h"


class Panel : public Interface {
public:
    Panel(sf::Vector2f pos, float width, float height, sf::Color background_color);

    sf::Color background_color;
    std::vector<std::shared_ptr<Interface>> elements;
    std::shared_ptr<InputField> focused_field;

    void Draw(sf::RenderWindow* window) override;
    void OnClick(sf::Vector2f mouse_pos) override;
    void OnHover(sf::Vector2f mouse_pos) override;
    void OnTextEntered(sf::Uint32 unicode) override;

    void AddElementWithRelativePos(std::shared_ptr<Interface>& element, sf::Vector2f relative_pos);
    void SetPosition(sf::Vector2f pos) override;
};


#endif //PANEL_H
