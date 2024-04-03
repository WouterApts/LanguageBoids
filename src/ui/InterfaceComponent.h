//
// Created by wouter on 25-3-2024.
//

#ifndef INTERFACECOMPONENT_H
#define INTERFACECOMPONENT_H
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Transformable.hpp>


class InterfaceComponent : public sf::Transformable {
public:
    bool active = true;
    bool mouse_inside = false;

    explicit InterfaceComponent(sf::Vector2f pos);

    void Activate();
    void Deactivate();
    void ToggleActivation();

    virtual bool IsPointInside(sf::Vector2f point) const { return false; };
    virtual void Draw(sf::RenderWindow* window) {};
    virtual void OnLeftClick(sf::Vector2f mouse_pos) {};
    virtual void OnRightClick(sf::Vector2f mouse_pos) {};
    virtual void OnMouseEnter(sf::Vector2f mouse_pos) {};
    virtual void OnMouseLeave(sf::Vector2f mouse_pos) {};
    virtual void OnKeyBoardEnter(sf::Uint32 unicode) {};

    virtual void SetPosition(const sf::Vector2f& pos);
    virtual void SetOrigin(const sf::Vector2f& pos);
};

class RectangleComponent : public InterfaceComponent {
public:
    sf::RectangleShape rect;
    sf::Color default_background_color;
    sf::Color default_outline_color;

    RectangleComponent(sf::Vector2f pos, sf::Vector2f size,
                       sf::Color background_color = sf::Color::Transparent,
                       sf::Color outline_color = sf::Color::Transparent,
                       float outline_thickness = 0.f);

    sf::FloatRect GetGlobalBoundsWithOrigin() const;

    bool IsPointInside(sf::Vector2f point) const override;
    void SetPosition(const sf::Vector2f& pos) override;
    void SetOrigin(const sf::Vector2f& pos) override;
};



#endif //INTERFACECOMPONENT_H
