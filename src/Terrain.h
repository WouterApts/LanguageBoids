//
// Created by wouter on 10-3-2024.
//

#ifndef TERRAIN_H
#define TERRAIN_H
#include <vector>
#include <Eigen/Dense>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Boid;

class Terrain {
public:
    Terrain(const std::vector<Eigen::Vector2f>& vertices, float friction_modifier, float struggle_modifier, float min_speed, float max_speed);
    ~Terrain() = default;

    bool IsPointInside(const Eigen::Vector2f& point) const;

    void ApplyEffects(Boid *boid) const;
    void Draw(sf::RenderWindow* window) const;

    std::vector<Eigen::Vector2f> vertices;
    sf::ConvexShape polygon;
    float friction_modifier;
    float struggle_modifier;
    float min_speed;
    float max_speed;
};



#endif //TERRAIN_H
