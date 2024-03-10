//
// Created by wouter on 10-3-2024.
//

#include "Terrain.h"
#include "boid.h"
#include <SFML/Graphics/RenderWindow.hpp>

Terrain::Terrain(const std::vector<Eigen::Vector2f>& vertices, float friction_modifier, float struggle_modifier)
: vertices(vertices), friction_modifier(friction_modifier), struggle_modifier(struggle_modifier) {
    polygon.setPointCount(vertices.size());
    for (int i = 0; i < vertices.size(); ++i) {
        polygon.setPoint(i, sf::Vector2f(vertices[i].x(), vertices[i].y()));
    }
    auto terrain_gray = sf::Color(50,50,50);
    polygon.setFillColor(terrain_gray);
}

bool Terrain::IsPointInside(const Eigen::Vector2f &point) const {
    int verticesCount = static_cast<int>(vertices.size());
    bool inside = false;

    for (int i = 0, j = verticesCount - 1; i < verticesCount; j = i++) {
        float intersection_x = (vertices[j].x() - vertices[i].x()) * (point.y() - vertices[i].y()) /
                               (vertices[j].y() - vertices[i].y()) + vertices[i].x();
        if (vertices[i].y() > point.y() != vertices[j].y() > point.y() && point.x() <= intersection_x) {
            inside = !inside;
        }
    }
    return inside;
}

void Terrain::ApplyEffects(Boid *boid) const {
    Eigen::Vector2f acceleration = -boid->vel.normalized() * friction_modifier * boid->max_speed;
    boid->SetAcceleration(boid->acc + acceleration);
}

void Terrain::Draw(sf::RenderWindow* window) const {
    window->draw(polygon);
}
