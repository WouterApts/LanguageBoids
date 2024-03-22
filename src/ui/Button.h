//
// Created by wouter on 17-3-2024.
//

#ifndef BUTTON_H
#define BUTTON_H
#include <functional>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Interface.h"

class Button : public Interface {
public:
    Button(std::function<void()> callback, sf::Vector2f pos, float width, float height);

    std::function<void()> callback;
    void OnHover(sf::Vector2f mouse_pos) override;
    void OnClick(sf::Vector2f mouse_pos) override;
    void Draw(sf::RenderWindow* window) override;
};

class ImageButton : public Button {
public:
    ImageButton(std::function<void()> callback, const std::string &texture_id, sf::Vector2f pos, float width, float height);
    void SetPosition(sf::Vector2f pos) override;

    sf::Sprite sprite;
    void Draw(sf::RenderWindow* window) override;
};

class TextButton : public Button {
public:
    TextButton(std::function<void()> callback, std::string text, sf::Color text_color, int text_size, sf::Color background_color,
               sf::Vector2f pos, float width, float height);

    int text_size;
    std::string text_string;
    sf::Color text_color;
    sf::Text text;
    sf::Color background_color;

    TextButton(std::function<void()> callback, const std::string& text_string, sf::Color text_color, int text_size,
               sf::Color background_color, sf::Vector2f pos, float width, float height);

    void Draw(sf::RenderWindow* window) override;
};



#endif //BUTTON_H
