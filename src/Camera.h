//
// Created by wouter on 20-2-2024.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>

class Camera {
public:

    Camera(sf::Vector2f center, float width, float height);
    ~Camera() = default;

    void StartDragging(sf::Vector2i mouse_pos);
    void StopDragging();

    sf::View view;
    sf::Vector2f center;
    float default_width;
    float default_height;
    float zoom{};

    bool dragging{};
    sf::Vector2i prev_mouse_pos;

    void Drag(sf::Vector2i mouse_pos);
    void Move(sf::Vector2f direction);
    void Zoom(float zoom_modifier);
};



#endif //CAMERA_H
