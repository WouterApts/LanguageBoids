//
// Created by wouter on 21-2-2024.
//

#ifndef OBSTACLES_H
#define OBSTACLES_H

#include <memory>
#include <optional>
#include <Eigen/Dense>
#include <SFML/Graphics.hpp>

// Forward declaration of Boid class
class Boid;

class Obstacle {
public:
    Obstacle() = default;
    virtual ~Obstacle() = default;

    virtual std::optional<Eigen::Vector2f> CalcCollisionNormal(Eigen::Vector2f pos, float collision_radius);
    virtual void Draw(sf::RenderWindow* window);
    virtual std::string ToString() const;

};

class LineObstacle : public Obstacle {
public:
    LineObstacle(Eigen::Vector2f& start, Eigen::Vector2f& end, float width, sf::Color color);
    std::optional<Eigen::Vector2f> CalcCollisionNormal(Eigen::Vector2f pos, float collision_radius) override;
    void Draw(sf::RenderWindow* window) override;
    std::string ToString() const override;

    static std::shared_ptr<LineObstacle> fromString(const std::string &str);

    std::vector<sf::Vertex> vertices;
    float thickness;
    sf::Color color;

    Eigen::Vector2f startPoint;
    Eigen::Vector2f endPoint;
    float length;
};


class CircleObstacle : public Obstacle {
public:
    CircleObstacle(Eigen::Vector2f& center, float radius, sf::Color color);
    std::optional<Eigen::Vector2f> CalcCollisionNormal(Eigen::Vector2f pos, float collision_radius) override;
    void Draw(sf::RenderWindow* window) override;
    std::string ToString() const override;

    static std::shared_ptr<CircleObstacle> FromString(const std::string &str);

    sf::CircleShape circle_shape;
    Eigen::Vector2f center;
    float radius;
    sf::Color color;
};

#endif // OBSTACLES_H

