//
// Created by wouter on 21-2-2024.
//

#include <utility>
#include "Eigen/Dense"
#include "Obstacles.h"

#include "Boid.h"


LineObstacle::LineObstacle(Eigen::Vector2f& start, Eigen::Vector2f& end)
    : startPoint(std::move(start)), endPoint(std::move(end)), length((start-end).norm()) {
}


Eigen::Vector2f LineObstacle::CalcCollisionNormal(const Boid* boid) {

    Eigen::Vector2f CA = startPoint - boid->pos;
    Eigen::Vector2f CB = endPoint - boid->pos;
    Eigen::Vector2f BA = startPoint - endPoint;
    Eigen::Vector2f AB = endPoint - startPoint;
    float CA_len = CA.norm();
    float CB_len = CB.norm();
    float min_distance_to_line;
    float max_distance_to_line = std::max(CA_len, CB_len);
    Eigen::Vector2f collision_normal;

    // Use dot product to check if circle-center projection is on line segment.
    if (CA.dot(BA) > 0 && CB.dot(AB) > 0) {
        float area = (CA.x()*CB.y() - CA.y()*CB.x()) / 2;
        min_distance_to_line = 2*area/length;
        collision_normal << -AB.y(), AB.x();
    // If not, the shortest distance is to one of the outer points of the line segment.
    } else {
        if (CA_len < CB_len) {
            collision_normal = -CA;
            min_distance_to_line = CA_len;
        } else {
            collision_normal = -CB;
            min_distance_to_line = CB_len;
        }
    }

    if (min_distance_to_line <= boid->collision_radius && max_distance_to_line >= boid->collision_radius) {
        return collision_normal.normalized();
    }
    return Eigen::Vector2f::Zero();
}


CircleObstacle::CircleObstacle(Eigen::Vector2f& center, float radius)
    : center(std::move(center)), radius(radius) {
}


Eigen::Vector2f CircleObstacle::CalcCollisionNormal(const Boid* boid) {
    Eigen::Vector2f direction = center - boid->pos;
    if (direction.norm() <= radius + boid->collision_radius) {
        return direction.normalized();
    }
    return Eigen::Vector2f::Zero();
}


