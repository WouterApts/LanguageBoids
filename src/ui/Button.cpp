//
// Created by wouter on 17-3-2024.
//

#include "Button.h"

#include <utility>
#include <SFML/Graphics/Text.hpp>
#include "ResourceManager.h"

Button::Button(std::function<void()> callback, sf::Vector2f pos, float width, float height)
    : Interface(pos, width, height), callback(std::move(callback)) { }

void Button::OnClick(sf::Vector2f mouse_pos) {
    callback();
}

void Button::OnHover(sf::Vector2f mouse_pos) {}

void Button::Draw(sf::RenderWindow *window) {}


ImageButton::ImageButton(std::function<void()> callback, const std::string &texture_id, sf::Vector2f pos, float width, float height)
    : Button(std::move(callback), pos, width, height) {
    auto p_texture = ResourceManager::GetTexture(texture_id);
    sprite.setTexture(*p_texture);
    sprite.setScale(width / p_texture->getSize().x, height / p_texture->getSize().y);
    sprite.setPosition(this->pos);
}

void ImageButton::SetPosition(sf::Vector2f pos) {
    this->pos = pos;
    sprite.setPosition(pos);
}

void ImageButton::Draw(sf::RenderWindow *window) {
    window->draw(sprite);
}


TextButton::TextButton(std::function<void()> callback, const std::string& text_string, sf::Color text_color, int text_size,
                       sf::Color background_color, sf::Vector2f pos, float width, float height)
    : Button(std::move(callback), pos, width, height), text_size(text_size), text_string(text_string), text_color(text_color) {

    // Set the rectangle border
    rect.setFillColor(background_color);
    rect.setOutlineColor(text_color);
    rect.setOutlineThickness(5.f);

    // Set the text properties
    text.setFont(*ResourceManager::GetFont("arial"));
    text.setString(text_string);
    text.setFillColor(text_color);
    text.setCharacterSize(text_size); // Set the character size

    // Calculate the position for the text to be centered in the rectangle
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = pos.x + (this->width - textBounds.width) / 2.0f;
    float textPosY = pos.y + (this->height - textBounds.height) / 2.0f - textBounds.top;
    text.setPosition(textPosX, textPosY);
}

void TextButton::SetPosition(sf::Vector2f pos) {
    // Calculate the position for the text to be centered in the rectangle
    this->pos = pos;
    rect.setPosition(this->pos);
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = pos.x + (this->width - textBounds.width) / 2.0f;
    float textPosY = pos.y + (this->height - textBounds.height) / 2.0f - textBounds.top;
    text.setPosition(textPosX, textPosY);
}

void TextButton::Draw(sf::RenderWindow *window) {
    window->draw(rect);
    window->draw(text);
}
