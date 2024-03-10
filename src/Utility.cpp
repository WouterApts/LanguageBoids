//
// Created by wouter on 25-2-2024.
//

#include "Utility.h"
#include <random>

/* Input Processing */
bool isKeyPressedOnce(sf::Keyboard::Key keyCode)
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


/*  Random geberators */
float getRandomFloatBetween(float min, float max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}
