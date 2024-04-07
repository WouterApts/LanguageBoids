//
// Created by wouter on 25-2-2024.
//

#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

class InterfaceManager;

bool IsKeyPressedOnce(sf::Keyboard::Key keyCode);

float GetRandomFloatBetween(float min, float max);

void PrintFPS(sf::Time delta_time);

void DisplayMessage(InterfaceManager interface_manager, sf::Vector2f pos, const std::string& message, sf::Time time, sf::Color text_color = sf::Color::White, int text_size = 20);
#endif //UTILITY_H
