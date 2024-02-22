//
// Created by wouter on 21-2-2024.
//

#ifndef OBSTACLES_H
#define OBSTACLES_H

#include <Eigen/Dense>

// Forward declaration of Boid class
class Boid;

class Obstacle {
public:
    Obstacle();
    virtual ~Obstacle();

    virtual Eigen::Vector2f CalcCollisionNormal(const Boid* boid);
};

class LineObstacle : public Obstacle {
public:
    LineObstacle(Eigen::Vector2f& start, Eigen::Vector2f& end);

    Eigen::Vector2f CalcCollisionNormal(const Boid* boid) override;

    Eigen::Vector2f startPoint;
    Eigen::Vector2f endPoint;
    float length;
};


class CircleObstacle : public Obstacle {
public:
    CircleObstacle(Eigen::Vector2f& center, float radius);

    Eigen::Vector2f CalcCollisionNormal(const Boid* boid) override;

    Eigen::Vector2f center;
    float radius;
};

#endif // OBSTACLES_H

