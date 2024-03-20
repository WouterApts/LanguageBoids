//
// Created by wouter on 17-3-2024.
//

#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <functional>
#include <variant>
#include <SFML/Graphics/Text.hpp>
#include "Interface.h"


class InputField : public Interface {
public:

    InputField(sf::Vector2f pos, float width, float height, int character_limit, std::string default_value);

    void Draw(sf::RenderWindow* window) override;
    void OnClick(sf::Vector2f mouse_pos) override;
    void OnHover(sf::Vector2f mouse_pos) override;

    void OnTextEntered(sf::Uint32 unicode) override;
    void SetFocus(bool focus);

    sf::RectangleShape m_cursor;
    sf::Text m_text;
    sf::Font font;
    int character_limit;
    bool m_focused{};
    std::string default_value;
};

class IntInputField : public InputField {
public:
    IntInputField(std::function<void(int)> callback, sf::Vector2f pos, float width, float height, int character_limit, std::string default_value);

    std::function<void(int)> callback;
    void OnTextEntered(sf::Uint32 unicode) override;
};

class FloatInputField : public InputField {
public:
    FloatInputField(std::function<void(float)> callback, sf::Vector2f pos, float width, float height, int character_limit, std::string default_value);

    std::function<void(float)> callback;
    void OnTextEntered(sf::Uint32 unicode) override;
};



#endif //TEXTBOX_H
