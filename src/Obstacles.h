//
// Created by wouter on 21-2-2024.
//

#ifndef OBSTACLES_H
#define OBSTACLES_H

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>

// Forward declaration of Boid class
class Boid;

class Obstacle {
public:
    Obstacle() = default;
    virtual ~Obstacle() = default;

    virtual Eigen::Vector2f CalcCollisionNormal(const Boid* boid);
    virtual void Draw(sf::RenderWindow* window);

};

class LineObstacle : public Obstacle {
public:
    LineObstacle(Eigen::Vector2f& start, Eigen::Vector2f& end, float width, sf::Color color);

    Eigen::Vector2f CalcCollisionNormal(const Boid* boid) override;
    void Draw(sf::RenderWindow* window) override;

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

    Eigen::Vector2f CalcCollisionNormal(const Boid* boid) override;
    void Draw(sf::RenderWindow* window) override;

    sf::CircleShape circle_shape;
    Eigen::Vector2f center;
    float radius;
    sf::Color color;
};

#endif // OBSTACLES_H

