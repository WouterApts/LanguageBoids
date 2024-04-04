//
// Created by wouter on 17-3-2024.
//

#include "Panel.h"

#include <iostream>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>

Panel::Panel(sf::Vector2f pos, sf::Vector2f size,
             sf::Color background_color,
             sf::Color outline_color,
             float outline_thickness)
    : RectangleComponent(pos, size, background_color, outline_color, outline_thickness) {
}

void Panel::Draw(sf::RenderWindow* window) {
    window->draw(rect);
    for (auto& component : components) {
        if (component->active) component->Draw(window);
    }
}

void Panel::OnLeftClick(sf::Vector2f mouse_pos) {
    for (const auto& component : components) {
        if (component->IsPointInside(mouse_pos) && component->active) {
            component->OnLeftClick(mouse_pos);
        }
    }
}

void Panel::OnMouseEnter(sf::Vector2f mouse_pos) {
    for (auto& element : components) {
        if (element->IsPointInside(mouse_pos) && !element->mouse_inside) {
            element->OnMouseEnter(mouse_pos);
            element->mouse_inside = true;
        }
    }
}

void Panel::OnMouseLeave(sf::Vector2f mouse_pos) {
    for (auto& element : components) {
        if (!element->IsPointInside(mouse_pos) && element->mouse_inside) {
            element->OnMouseLeave(mouse_pos);
            element->mouse_inside = false;
        }
    }
}

void Panel::OnKeyBoardEnter(sf::Uint32 unicode) {
    for (auto& element : components) {
        element->OnKeyBoardEnter(unicode);
    }
}

void Panel::AddComponentWithRelativePos(const std::shared_ptr<InterfaceComponent>& component, sf::Vector2f relative_pos) {
    relative_position.push_back(relative_pos);
    components.push_back(component);
    component->SetPosition(this->getPosition() - this->getOrigin() + relative_pos);
}

void Panel::SetPosition(const sf::Vector2f &pos) {
    RectangleComponent::SetPosition(pos);
    for (int i = 0; i < components.size(); ++i) {
        components[i]->SetPosition(this->getPosition() - this->getOrigin() + relative_position[i]);
    }
}
