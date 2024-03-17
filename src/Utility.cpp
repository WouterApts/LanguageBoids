//
// Created by wouter on 25-2-2024.
//

#include "Utility.h"
#include <iostream>
#include <random>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

/* Input Processing */
bool IsKeyPressedOnce(sf::Keyboard::Key keyCode)
{
    static bool keyPressed[sf::Keyboard::KeyCount] = {false};

    if (sf::Keyboard::isKeyPressed(keyCode)) {
        if (!keyPressed[keyCode]) {
            keyPressed[keyCode] = true;
            return true;
        }
    }
    else {
        keyPressed[keyCode] = false;
    }
    return false;
}


/*  Random generators */
float GetRandomFloatBetween(float min, float max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}


void PrintFPS(sf::Time delta_time) {
    static auto fps_timer = sf::Time::Zero;
    static float frame_counter = 0.f;

    fps_timer += delta_time;
    frame_counter++;
    if (fps_timer > sf::seconds(1.f)) {
        std::cout << frame_counter << std::endl;
        frame_counter = 0;
        fps_timer -= sf::seconds(1.f);
    }
}


