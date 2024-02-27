//
// Created by wouter on 17-2-2024.
//

#ifndef THESIS_CONFIG_H
#define THESIS_CONFIG_H

#include <Eigen/Dense>

constexpr int LANGUAGE_SIZE = 50;
inline float LANGUAGE_MAX_DIFF = static_cast<float>(Eigen::VectorXi::Ones(LANGUAGE_SIZE).squaredNorm());
constexpr float LANGUAGE_MUTATION_RATE = 0.01f;

constexpr float COHERENCE_FACTOR = 1.f;
constexpr float ALIGNMENT_FACTOR = 0.3f;
constexpr float SEPARATION_FACTOR = 1.f;

constexpr float MAX_SPEED = 125;
constexpr float MAX_SPEED_SQUARED = MAX_SPEED * MAX_SPEED;
constexpr float MIN_SPEED = 75;
constexpr float MIN_SPEED_SQUARED = MIN_SPEED * MIN_SPEED;
constexpr float PERCEPTION_RADIUS = 250;
constexpr float AVOIDANCE_RADIUS = 100;

constexpr float BOID_COLLISON_RADIUS = 10;
constexpr float RESTITUTION_COEFFICIENT = 1;

#endif //THESIS_CONFIG_H
