//
// Created by wouter on 16-3-2024.
//

#ifndef USERINTERFACE_H
#define USERINTERFACE_H
#include <functional>
#include <string>
#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>


class Interface {
public:
    Interface();

    bool active;
    void Draw();
};

class Window : Interface {
public:
    Window();

    sf::Vector2f pos;
    float width;
    float height;
    std::vector<Interface> elements;
    void Draw();
};

class Button : Interface {
public:
    Button(std::function<void()> callback, sf::Texture* p_image);

    sf::Sprite sprite;
    sf::Vector2f pos;
    float width;
    float height;
    void Draw();
};



#endif //USERINTERFACE_H
