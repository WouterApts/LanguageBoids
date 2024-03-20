//
// Created by wouter on 21-2-2024.
//

#include <utility>
#include "Eigen/Dense"
#include "Obstacles.h"

#include <iostream>
#include "Boid.h"


LineObstacle::LineObstacle(Eigen::Vector2f& start, Eigen::Vector2f& end, float width, sf::Color color)
    : startPoint(std::move(start)), endPoint(std::move(end)), length((start-end).norm()),
      thickness(width), color(color) {

    auto direction = (startPoint - endPoint);
    Eigen::Vector2f offset = Eigen::Vector2f(-direction.y(), direction.x()).normalized() * (width/2.0f);

    Eigen::Vector2f p1 = startPoint + offset;
    Eigen::Vector2f p2 = startPoint - offset;
    Eigen::Vector2f p3 = endPoint - offset;
    Eigen::Vector2f p4 = endPoint + offset;
    vertices.emplace_back(sf::Vector2f(p1.x(), p1.y()), color);
    vertices.emplace_back(sf::Vector2f(p2.x(), p2.y()), color);
    vertices.emplace_back(sf::Vector2f(p3.x(), p3.y()), color);
    vertices.emplace_back(sf::Vector2f(p4.x(), p4.y()), color);
}

void LineObstacle::Draw(sf::RenderWindow* window) {
    window->draw(vertices.data(),vertices.size(),sf::Quads);
}

std::optional<Eigen::Vector2f> LineObstacle::CalcCollisionNormal(Eigen::Vector2f pos, float collision_radius) {
    Eigen::Vector2f CA = pos - startPoint;
    Eigen::Vector2f CB = pos - endPoint;
    Eigen::Vector2f AB = endPoint - startPoint;

    float CA_len = CA.norm();
    float CB_len = CB.norm();

    float min_distance_to_line;
    float max_distance_to_line = AB.norm(); // Use the length of AB as max distance

    Eigen::Vector2f collision_normal;

    // Calculate the length of the line segment
    float length = AB.norm();

    // Calculate the signed area of the triangle formed by CAB
    float area = std::abs(CA.x() * CB.y() - CB.x() * CA.y()) / 2;

    // Check if the projection of the boid's position onto the line segment falls within the segment
    if (CA.dot(AB) > 0 && CB.dot(-AB) > 0) {
        // Calculate the shortest distance from the boid to the line (line segment)
        min_distance_to_line = area / length;
        collision_normal << -AB.y(), AB.x();

        // Reverse the direction of the normal if necessary
        if (CA.dot(collision_normal) < 0) {
            collision_normal *= -1;
        }
    // Else, the projection is outside the line segment, calculate the distance to the nearest endpoint
    } else {

        if (CA_len < CB_len) {
            collision_normal = -CA.normalized();
            min_distance_to_line = CA_len;
        } else {
            collision_normal = -CB.normalized();
            min_distance_to_line = CB_len;
        }
    }

    // Check if the boid collides with the line obstacle
    if (min_distance_to_line <= collision_radius && max_distance_to_line >= collision_radius) {
        return collision_normal.normalized();
    }

    // If there is no collision, return a zero vector
    return std::nullopt;
}

CircleObstacle::CircleObstacle(Eigen::Vector2f& center, float radius, sf::Color color)
    : center(center), radius(radius), color(color) {
    circle_shape.setRadius(this->radius);
    circle_shape.setPosition(this->center.x()-this->radius, this->center.y()-this->radius);
    circle_shape.setFillColor(color);
}

void CircleObstacle::Draw(sf::RenderWindow* window) {
    window->draw(circle_shape);
}

std::optional<Eigen::Vector2f> CircleObstacle::CalcCollisionNormal(Eigen::Vector2f pos, float collision_radius) {
    Eigen::Vector2f distance_vector = pos - center;
    if (distance_vector.norm() <= radius + collision_radius) {
        return distance_vector.normalized();
    }
    return std::nullopt;
}


