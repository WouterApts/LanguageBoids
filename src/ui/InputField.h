//
// Created by wouter on 17-3-2024.
//

#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <functional>
#include <variant>
#include <SFML/Graphics/Text.hpp>
#include "Interface.h"
#include "InterfaceComponent.h"


class InputField : public RectangleComponent {
public:

    InputField(sf::Vector2f pos, sf::Vector2f size, int character_limit, std::string default_value);

    void Draw(sf::RenderWindow* window) override;
    void OnLeftClick(sf::Vector2f mouse_pos) override;
    void OnMouseEnter(sf::Vector2f mouse_pos) override;

    void OnKeyBoardEnter(sf::Uint32 unicode) override;
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
    std::function<void(int)> callback;

    IntInputField(std::function<void(int)> callback, sf::Vector2f pos, sf::Vector2f size, int character_limit,
                  std::string default_value);

    void OnKeyBoardEnter(sf::Uint32 unicode) override;
};

class FloatInputField : public InputField {
public:
    std::function<void(float)> callback;

    FloatInputField(std::function<void(float)> callback, sf::Vector2f pos, sf::Vector2f size, int character_limit,
                    std::string default_value);

    void OnKeyBoardEnter(sf::Uint32 unicode) override;
};


#endif //TEXTBOX_H
