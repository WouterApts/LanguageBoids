//
// Created by wouter on 16-3-2024.
//

#include "Interface.h"

#include <iostream>

Interface::Interface(sf::Vector2f pos, float width, float height)
    : pos(pos), width(width), height(height) {
    rect = sf::RectangleShape(sf::Vector2f(this->width, this->height));
    rect.setPosition(this->pos);
}

bool Interface::IsPointInsideRect(sf::Vector2f point) const {
    return (point.x > pos.x && point.x < pos.x + width &&
            point.y > pos.y && point.y < pos.y + height);
}

void Interface::SetPosition(sf::Vector2f pos) {
    this->pos = pos;
    rect.setPosition(this->pos);
}

void Interface::SetSize(float width, float height) {
    this->width = width;
    this->height = height;
    rect = sf::RectangleShape(sf::Vector2f(this->width, this->height));
}

