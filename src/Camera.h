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

    void StartDragging(sf::Vector2f current_pos);
    void StopDragging();

    sf::View view;
    sf::Vector2f center;
    float width;
    float height;
    float zoom{};

    bool dragging{};
    sf::Vector2f prev_pos;

    void Drag(sf::Vector2f mouse_pos);
    void Move(sf::Vector2f direction);
    void Zoom(float zoom_modifier);
};



#endif //CAMERA_H
