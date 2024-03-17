//
// Created by wouter on 16-3-2024.
//

#ifndef USERINTERFACE_H
#define USERINTERFACE_H
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>


class Interface {
public:
    Interface(sf::Vector2f pos, float width, float height);
    virtual ~Interface() = default;

    bool active{};
    sf::Vector2f pos;
    float width;
    float height;
    sf::RectangleShape rect;

    bool IsPointInsideRect(sf::Vector2f point) const;
    void SetSize(float width, float height);
    virtual void SetPosition(sf::Vector2f pos);

    virtual void Draw(sf::RenderWindow* window) {};
    virtual void OnClick(sf::Vector2f mouse_pos) {};
    virtual void OnHover(sf::Vector2f mouse_pos) {};
};

#endif //USERINTERFACE_H
