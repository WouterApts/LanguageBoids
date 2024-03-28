//
// Created by wouter on 25-3-2024.
//

#include "InterfaceComponent.h"

InterfaceComponent::InterfaceComponent(sf::Vector2f pos) {
    InterfaceComponent::SetPosition(pos);
}

void InterfaceComponent::Activate() {
    this->active = true;
}

void InterfaceComponent::Deactivate() {
    this->active = false;
}

void InterfaceComponent::ToggleActivation() {
    this->active = !this->active;
}

void InterfaceComponent::SetPosition(const sf::Vector2f &pos) {
    sf::Transformable::setPosition(pos);
}

void InterfaceComponent::SetOrigin(const sf::Vector2f &pos) {
    sf::Transformable::setOrigin(pos);
}

RectangleComponent::RectangleComponent(sf::Vector2f pos, sf::Vector2f size,
                                       sf::Color background_color,
                                       sf::Color outline_color,
                                       float outline_thickness)
    : InterfaceComponent(pos), default_background_color(background_color), default_outline_color(outline_color) {
    rect.setPosition(pos);
    rect.setSize(size);
    rect.setFillColor(background_color);
    rect.setOutlineColor(outline_color);
    rect.setOutlineThickness(outline_thickness);
}

sf::FloatRect RectangleComponent::GetGlobalBoundsWithOrigin() const {
    sf::FloatRect bounds = this->rect.getGlobalBounds();
    bounds.left -= this->rect.getOrigin().x;
    bounds.top -= this->rect.getOrigin().y;
    return bounds;
}

bool RectangleComponent::IsPointInside(sf::Vector2f point) const {
    auto bounds = GetGlobalBoundsWithOrigin();
    return bounds.contains(point);
}

void RectangleComponent::SetPosition(const sf::Vector2f &pos) {
    InterfaceComponent::SetPosition(pos);
    rect.setPosition(pos);
}

void RectangleComponent::SetOrigin(const sf::Vector2f &pos) {
    InterfaceComponent::SetOrigin(pos);
    rect.setOrigin(pos);
}
