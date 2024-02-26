//
// Created by wouter on 25-2-2024.
//

#include "Utility.h"

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
