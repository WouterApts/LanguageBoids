//
// Created by wouter on 19-3-2024.
//

#include "TextField.h"

TextField::TextField(sf::Vector2f pos, int size, const std::string &text, const sf::Font &font, sf::Color color)
    : InterfaceComponent(pos), font(font), color(color), size(size) {
    this->text.setString(text);
    this->text.setFont(font);
    this->text.setCharacterSize(size);
    this->text.setFillColor(color);
}

void TextField::Draw(sf::RenderWindow *window) {
    text.setPosition(getPosition());
    window->draw(text);
}
