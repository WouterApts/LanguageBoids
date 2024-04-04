//
// Created by wouter on 17-3-2024.
//

#include "Button.h"

#include <iostream>
#include <utility>
#include <SFML/Graphics/Text.hpp>
#include "ResourceManager.h"

Button::Button(std::function<void()> callback, sf::Vector2f pos, sf::Vector2f size, sf::Color background_color,
               sf::Color outline_color, float outline_thickness)
    : RectangleComponent(pos, size, background_color, outline_color, outline_thickness), callback(std::move(callback)) {
}

void Button::OnLeftClick(sf::Vector2f mouse_pos) {
    callback();
}

void Button::Draw(sf::RenderWindow *window) {}


ImageButton::ImageButton(std::function<void()> callback, const std::string &texture_id, sf::Vector2f pos,
                         sf::Vector2f size, sf::Color background_color, sf::Color outline_color, float outline_thickness)
    : Button(std::move(callback), pos, size, background_color, outline_color, outline_thickness) {

    auto p_texture = ResourceManager::GetTexture(texture_id);
    sprite.setTexture(*p_texture);
    sprite.setScale(this->rect.getSize().x / p_texture->getSize().x,
                    this->rect.getSize().y / p_texture->getSize().y);
    sprite.setPosition(this->getPosition());
}

void ImageButton::SetPosition(const sf::Vector2f& pos) {
    RectangleComponent::SetPosition(pos);
    sprite.setPosition(pos);
}

void ImageButton::SetOrigin(const sf::Vector2f& pos) {
    RectangleComponent::SetOrigin(pos);
    sprite.setOrigin(pos);
}

void ImageButton::Draw(sf::RenderWindow *window) {
    window->draw(rect);
    window->draw(sprite);
}


TextButton::TextButton(std::function<void()> callback, const std::string& text_string,  sf::Vector2f pos, sf::Vector2f size,
                       int text_size, sf::Color text_color, sf::Color background_color, sf::Color outline_color, float outline_thickness)
    : Button(std::move(callback), pos, size, background_color, outline_color, outline_thickness),
      text_size(text_size), text_string(text_string), text_color(text_color) {

    // Set the text properties
    text.setFont(*ResourceManager::GetFont("arial"));
    text.setString(text_string);
    text.setFillColor(text_color);
    text.setCharacterSize(text_size); // Set the character size

    // Calculate the position for the text to be centered in the rectangle
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = pos.x + (this->rect.getSize().x - textBounds.width) / 2.0f;
    float textPosY = pos.y + (this->rect.getSize().y - textBounds.height) / 2.0f - textBounds.top;
    text.setPosition(static_cast<int>(textPosX), static_cast<int>(textPosY));
}

void TextButton::OnMouseEnter(sf::Vector2f mouse_pos) {
    auto color = rect.getFillColor();
    color.a += 50;
    rect.setFillColor(color);
}

void TextButton::OnMouseLeave(sf::Vector2f mouse_pos) {
    rect.setFillColor(default_background_color);
}

void TextButton::SetPosition(const sf::Vector2f& pos) {
    std::cout << "setting position" << std::endl;
    RectangleComponent::SetPosition(pos);
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = pos.x + (this->rect.getSize().x - textBounds.width) / 2.0f;
    float textPosY = pos.y + (this->rect.getSize().y - textBounds.height) / 2.0f - textBounds.top;
    text.setPosition(static_cast<int>(textPosX), static_cast<int>(textPosY));
}

void TextButton::SetOrigin(const sf::Vector2f &pos) {
    RectangleComponent::SetOrigin(pos);
}

void TextButton::Draw(sf::RenderWindow *window) {
    window->draw(rect);
    window->draw(text);
}

