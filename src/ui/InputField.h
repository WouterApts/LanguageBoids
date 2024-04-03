//
// Created by wouter on 17-3-2024.
//

#ifndef TEXTBOX_H
#define TEXTBOX_H
#include <functional>
#include <SFML/Graphics/Text.hpp>
#include "Interface.h"
#include "InterfaceComponent.h"
#include "InterfaceManager.h"


class InputField : public RectangleComponent {
public:

    InputField(InterfaceManager& interface_manager, sf::Vector2f pos, sf::Vector2f size, int character_limit, std::string default_value);

    void Draw(sf::RenderWindow* window) override;
    void OnLeftClick(sf::Vector2f mouse_pos) override;
    void OnMouseEnter(sf::Vector2f mouse_pos) override;

    void OnKeyBoardEnter(sf::Uint32 unicode) override;
    void SetFocus(bool focus);

    InterfaceManager& interface_manager;
    sf::RectangleShape m_cursor;
    sf::Text m_text;
    sf::Font font;
    int character_limit;
    bool m_focused = false;
    std::string default_value;
};

class IntInputField : public InputField {
public:
    std::function<void(int)> callback;

    IntInputField(InterfaceManager& interface_manager, std::function<void(int)> callback, sf::Vector2f pos, sf::Vector2f size, int character_limit,
                  int default_value);

    void OnKeyBoardEnter(sf::Uint32 unicode) override;
};

class FloatInputField : public InputField {
public:
    std::function<void(float)> callback;

    FloatInputField(InterfaceManager& interface_manager, std::function<void(float)> callback, sf::Vector2f pos, sf::Vector2f size, int character_limit,
                    float default_value);

    void OnKeyBoardEnter(sf::Uint32 unicode) override;
};


#endif //TEXTBOX_H
