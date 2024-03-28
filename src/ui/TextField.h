//
// Created by wouter on 19-3-2024.
//

#ifndef TEXTFIELD_H
#define TEXTFIELD_H
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include "InterfaceComponent.h"

class TextField : public InterfaceComponent {
public:
    sf::Font font;
    sf::Text text;
    sf::Color color;
    int size;

    TextField(sf::Vector2f pos, int size, const std::string &text, const sf::Font &font, sf::Color color);
    void Draw(sf::RenderWindow *window) override;

};



#endif //TEXTFIELD_H
