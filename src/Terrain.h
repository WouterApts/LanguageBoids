//
// Created by wouter on 10-3-2024.
//

#ifndef TERRAIN_H
#define TERRAIN_H
#include <memory>
#include <vector>
#include <Eigen/Dense>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "boid.h"

class Boid;

class Terrain {
public:
    Terrain(const std::vector<Eigen::Vector2f>& vertices, float friction_modifier, const std::pair<int, float> &language_status_modifier, float min_speed, float max_speed);
    ~Terrain() = default;

    bool IsPointInside(const Eigen::Vector2f& point) const;

    void ApplyMovementEffects(Boid *boid) const;
    void Draw(sf::RenderWindow* window) const;

    std::string ToString() const;

    void ApplyLanguageStatusEffects(CompBoid *boid) const;

    void InitLanguageStatusMap(const std::map<int, float> &map);

    static std::shared_ptr<Terrain> FromString(const std::string &str) ;

    std::vector<Eigen::Vector2f> vertices;
    sf::ConvexShape polygon;
    float friction_modifier;
    float min_speed;
    float max_speed;

    std::pair<int, float> language_status_modifier;
    std::shared_ptr<std::map<int, float>> language_status_map;
};



#endif //TERRAIN_H
