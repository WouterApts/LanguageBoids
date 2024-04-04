//
// Created by wouter on 17-3-2024.
//

#include "InputField.h"

#include <format>
#include <iostream>
#include <utility>

#include "../InterfaceManager.h"
#include "../../ResourceManager.h"

InputField::InputField(InterfaceManager& interface_manager, sf::Vector2f pos, sf::Vector2f size, int character_limit, std::string default_value)
    : RectangleComponent(pos, size, sf::Color::White), interface_manager(interface_manager), character_limit(character_limit), default_value(std::move(default_value)) {

    font = *ResourceManager::GetFont("arial");
    m_text.setFont(font);
    m_text.setString(this->default_value);
}

void InputField::Draw(sf::RenderWindow* window) {
    // Draw the box
    window->draw(rect);
    // Draw the text
    m_text.setCharacterSize(2.f/3.f * rect.getSize().y);
    m_text.setFillColor(sf::Color::Black);
    m_text.setPosition(this->getPosition().x + 5.f, this->getPosition().y + this->rect.getSize().y / 4.f);
    window->draw(m_text);

    // Draw the cursor if focused
    if (m_focused) {
        // Calculate cursor position based on text width
        float textWidth = m_text.getLocalBounds().width;
        float cursorX = this->getPosition().x + 5.f + textWidth;

        // Draw the cursor
        m_cursor.setSize(sf::Vector2f(1.f, this->rect.getSize().y - 4.f));
        m_cursor.setFillColor(sf::Color::Black);
        m_cursor.setPosition(cursorX, this->getPosition().y + 2.f);
        window->draw(m_cursor);
    }
}

void InputField::OnLeftClick(sf::Vector2f mouse_pos) {
    if (GetGlobalBoundsWithOrigin().contains(mouse_pos)) {
        if (interface_manager.focused_input_field) {
            interface_manager.focused_input_field->SetFocus(false);
        }
        SetFocus(true);
        interface_manager.focused_input_field = this;
    }
}

void InputField::OnMouseEnter(sf::Vector2f mouse_pos) {
    RectangleComponent::OnMouseEnter(mouse_pos);
}

void InputField::OnKeyBoardEnter(sf::Uint32 unicode) {
    if (m_focused && active) {
        std::string currentText = m_text.getString();

        // Check if character limit is reached
        if (unicode == 8) { // Backspace
            if (!currentText.empty())
                currentText.pop_back();
        } else if (currentText.size() < character_limit) {
            currentText += static_cast<char>(unicode);
        }
        m_text.setString(currentText);
    }
}

void InputField::SetFocus(bool focus) {
    m_focused = focus;
    if (!m_focused & m_text.getString().isEmpty()) {
        rect.setFillColor(sf::Color::White);
        m_text.setString(default_value);
    }
}


// Integers //
IntInputField::IntInputField(InterfaceManager& interface_manager, std::function<void(int)> callback, sf::Vector2f pos, sf::Vector2f size, int character_limit, int default_value)
    : InputField(interface_manager, pos, size, character_limit, std::to_string(default_value)), callback(std::move(callback)), value(default_value) {
}

void IntInputField::OnKeyBoardEnter(sf::Uint32 unicode) {
    if (m_focused) {
        std::string currentText = m_text.getString();
        // Check if character limit is reached
        if (unicode == 8) { // Backspace
            if (!currentText.empty())
                currentText.pop_back();
        } else if (currentText.size() < character_limit && (unicode >= '0' && unicode <= '9')) {
            currentText += static_cast<char>(unicode);
            rect.setFillColor(sf::Color::White);
        }

        if (!currentText.empty()) {
            // Call the callback only when the input is a valid integer
            try {
                value = std::stoi(currentText);
                if (callback) callback(value);
            } catch (std::invalid_argument&) {
                rect.setFillColor(sf::Color(255, 200, 200));
            }
        } else {
            callback(std::stoi(default_value));
            rect.setFillColor(sf::Color(255, 200, 200));
        }
        m_text.setString(currentText);
    }
}


FloatInputField::FloatInputField(InterfaceManager& interface_manager, std::function<void(float)> callback, sf::Vector2f pos, sf::Vector2f size, int character_limit, float default_value)
    : InputField(interface_manager, pos, size, character_limit, std::format("{:.1f}", default_value)), callback(std::move(callback)), value(default_value) {
}

void FloatInputField::OnKeyBoardEnter(sf::Uint32 unicode) {
    if (m_focused) {
        std::string currentText = m_text.getString();
        // Check if character limit is reached
        if (unicode == 8) { // Backspace
            if (!currentText.empty())
                currentText.pop_back();
        } else if (currentText.size() < character_limit && ((unicode >= '0' && unicode <= '9')) || unicode == '.') {
            currentText += static_cast<char>(unicode);
            rect.setFillColor(sf::Color::White);
        }

        if (!currentText.empty()) {
            // Call the callback only when the input is a valid integer
            try {
                value = std::stof(currentText);
                if (callback) callback(value);
            } catch (std::invalid_argument&) {
                rect.setFillColor(sf::Color(255, 200, 200));
            }
        } else {
            callback(std::stof(default_value));
            rect.setFillColor(sf::Color(255, 200, 200));
        }
        m_text.setString(currentText);
    }
}
