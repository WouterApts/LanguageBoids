//
// Created by wouter on 17-2-2024.
//

#ifndef THESIS_BOID_H
#define THESIS_BOID_H

#include <memory>
#include <stdexcept>

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Obstacles.h"
#include "World.h"

class Boid {
private:
    std::shared_ptr<sf::Texture> p_texture;

public:

    Boid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
         float perception_radius = PERCEPTION_RADIUS, float avoidance_radius = AVOIDANCE_RADIUS, float collision_radius = BOID_COLLISON_RADIUS);

    virtual ~Boid() = default;

    sf::Sprite sprite;

    Eigen::Vector2f pos;
    Eigen::Vector2f vel;
    Eigen::Vector2f acc;
    float perception_radius;
    float avoidance_radius;
    float collision_radius;
    float max_speed;
    int spatial_key;

    void UpdateSprite();
    void SetPosition(Eigen::Vector2f position);
    void SetVelocity(Eigen::Vector2f velocity);
    void SetAcceleration(Eigen::Vector2f acceleration);

    void UpdatePosition(const sf::Time &delta_time);
    void UpdateVelocity(const std::vector<std::shared_ptr<Obstacle>> &obstacles, const sf::Time &delta_time);
    Eigen::Vector2f AvoidBorders(float width, float height) const;

};

class CompBoid : public Boid {
public:
    void UpdateLanguage(const std::vector<Boid *> &boids, sf::Time delta_time);

    void UpdateAcceleration(const std::vector<Boid *> &nearby_boids, World &world);

    int language_id;


};

class EvoBoid : public Boid {
public:

    EvoBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
            Eigen::VectorXi language, float language_influence,
            float perception_radius = PERCEPTION_RADIUS, float avoidance_radius = AVOIDANCE_RADIUS, float collision_radius = BOID_COLLISON_RADIUS);

    Eigen::VectorXi language_vector;
    float language_influence{};

    void UpdateAcceleration(const std::vector<EvoBoid*>& nearby_boids, World& world);
    Eigen::Vector2f CalcAvoidanceAcceleration(const std::vector<EvoBoid*>& nearby_boids, Eigen::VectorXf language_similarities) const;
    Eigen::Vector2f CalcSeparationAcceleration(const std::vector<EvoBoid*>& nearby_boids) const;
    Eigen::Vector2f CalcCoherenceAlignmentAcceleration(const std::vector<EvoBoid*>& nearby_boids, const Eigen::VectorXf& language_similarities) const;

    void UpdateLanguage(const std::vector<EvoBoid*> &boids, sf::Time delta_time);
    Eigen::VectorXf CalcLanguageSimilarities(const std::vector<EvoBoid *> &boids) const;
};

#endif //THESIS_BOID_H