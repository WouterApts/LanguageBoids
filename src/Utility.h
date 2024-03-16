//
// Created by wouter on 25-2-2024.
//

#ifndef HELPER_H
#define HELPER_H
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>

bool IsKeyPressedOnce(sf::Keyboard::Key keyCode);

float GetRandomFloatBetween(float min, float max);

void PrintFPS(sf::Time delta_time);

#endif //HELPER_H
