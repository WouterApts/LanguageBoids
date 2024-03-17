//
// Created by wouter on 17-3-2024.
//

#include "Panel.h"

#include <iostream>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>

Panel::Panel(sf::Vector2f pos, float width, float height, sf::Color background_color)
    : Interface(pos, width, height), background_color(background_color) {
    rect.setFillColor(this->background_color);
}

void Panel::Draw(sf::RenderWindow* window) {
    window->draw(rect);
    for (auto& element : elements) {
        element->Draw(window);
    }
}

void Panel::OnClick(sf::Vector2f mouse_pos) {
    for (auto& element : elements) {
        if (element->IsPointInsideRect(mouse_pos)) {
            element->OnClick(mouse_pos);
        }
    }
}

void Panel::OnHover(sf::Vector2f mouse_pos) {
    for (auto& element : elements) {
        if (element->IsPointInsideRect(mouse_pos)) {
            element->OnHover(mouse_pos);
        }
    }
}

void Panel::AddElementWithRelativePos(std::shared_ptr<Interface>& element, sf::Vector2f relative_pos) {
    element->SetPosition(this->pos + relative_pos);
    elements.push_back(element);
}

void Panel::SetPosition(sf::Vector2f pos) {
    sf::Vector2f shift = pos - this->pos;
    this->pos = pos;
    rect.setPosition(this->pos);
    for (auto& element : elements) {
        element->SetPosition(element->pos + shift);
    }
}
