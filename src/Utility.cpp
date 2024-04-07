//
// Created by wouter on 25-2-2024.
//

#include "Utility.h"
#include <iostream>
#include <random>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>

#include "ResourceManager.h"
#include "ui/InterfaceManager.h"
#include "ui/components/TextField.h"

namespace sf {
    class RenderWindow;
}

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

void DisplayMessage(InterfaceManager interface_manager, sf::Vector2f pos, const std::string& message, sf::Time time, sf::Color text_color, int text_size) {
    auto text_fld = std::make_shared<TextField>(pos, text_size, message, *ResourceManager::GetFont("arial"), text_color);

    interface_manager.AddComponent(text_fld);
}



