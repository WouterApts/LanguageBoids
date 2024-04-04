//
// Created by wouter on 17-3-2024.
//

#ifndef BUTTON_H
#define BUTTON_H
#include <functional>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "InterfaceComponent.h"

class Button : public RectangleComponent {
public:
    std::function<void()> callback;

    Button(std::function<void()> callback, sf::Vector2f pos, sf::Vector2f size, sf::Color background_color,
           sf::Color outline_color, float outline_thickness);

    void OnLeftClick(sf::Vector2f mouse_pos) override;
    void Draw(sf::RenderWindow* window) override;
};

class ImageButton : public Button {
public:
    sf::Sprite sprite;

    ImageButton(std::function<void()> callback, const std::string &texture_id,
                sf::Vector2f pos,
                sf::Vector2f size,
                sf::Color background_color = sf::Color::Transparent,
                sf::Color outline_color = sf::Color::Transparent,
                float outline_thickness = 0.f);

    void SetPosition(const sf::Vector2f& pos) override;
    void SetOrigin(const sf::Vector2f& pos) override;

    void Draw(sf::RenderWindow* window) override;
};

class TextButton : public Button {
public:
    int text_size;
    std::string text_string;
    sf::Color text_color;
    sf::Text text;

    TextButton(std::function<void()> callback, const std::string &text_string, sf::Vector2f pos, sf::Vector2f size, int text_size,
               sf::Color text_color = sf::Color::White,
               sf::Color background_color = sf::Color::Transparent,
               sf::Color outline_color = sf::Color::White,
               float outline_thickness = 3.f);

    void OnMouseEnter(sf::Vector2f mouse_pos) override;
    void OnMouseLeave(sf::Vector2f mouse_pos) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetOrigin(const sf::Vector2f& pos) override;

    void Draw(sf::RenderWindow* window) override;
};



#endif //BUTTON_H
