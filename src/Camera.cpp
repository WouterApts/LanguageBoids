//
// Created by wouter on 20-2-2024.
//

#include "Camera.h"
#include "SFML/Graphics.hpp"

Camera::Camera(sf::Vector2f center, float width, float height) : center(center), default_width(width), default_height(height) {
    zoom = 1;
    view.setCenter(center);
    view.setSize(width, height);
    dragging = false;
}

void Camera::StartDragging(sf::Vector2i mouse_pos) {
    dragging = true;
    prev_mouse_pos = mouse_pos;
}

void Camera::StopDragging() {
    dragging = false;
}

void Camera::FitWorld(const World &world) {
    float max_zoom = std::max((world.height*1.1 / default_height), world.width*1.1 / default_width);
    SetZoom(static_cast<float>(static_cast<int>(max_zoom * 10)) / 10.f);
}

void Camera::Drag(sf::Vector2i mouse_pos) {
    if (dragging) {
        sf::Vector2i direction = (prev_mouse_pos - mouse_pos);
        direction.x *= zoom;
        direction.y *= zoom;
        prev_mouse_pos = mouse_pos;
        Move(static_cast<sf::Vector2f>(direction));
    }
}

void Camera::Move(sf::Vector2f direction) {
    center = center + direction;
    view.move(direction);
}

void Camera::Zoom(float zoom_modifier) {
    zoom = std::max(0.2f, zoom + zoom_modifier);
    view.setSize(default_width * zoom, default_height * zoom);
}

void Camera::SetZoom(float zoom_value) {
    zoom = std::max(0.2f, zoom_value);
    view.setSize(default_width * zoom, default_height * zoom);
}
