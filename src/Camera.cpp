//
// Created by wouter on 20-2-2024.
//

#include "Camera.h"
#include "SFML/Graphics.hpp"

Camera::Camera(sf::Vector2f center, float width, float height) : center(center), width(width), height(height) {
    zoom = 1;
    view.setCenter(center);
    view.setSize(width, height);
    dragging = false;
}

void Camera::StartDragging(sf::Vector2f current_pos) {
    dragging = true;
    prev_pos = current_pos;
}

void Camera::StopDragging() {
    dragging = false;
}

void Camera::Drag(sf::Vector2f current_pos) {
    if (dragging) {
        sf::Vector2f direction = (current_pos - prev_pos);
        prev_pos = current_pos;
        Move(direction);
    }
}

void Camera::Move(sf::Vector2f direction) {
    center = center + direction;
    view.move(direction);
}

void Camera::Zoom(float zoom_modifier) {
    zoom = zoom + zoom_modifier;
    view.zoom(zoom_modifier);
}
