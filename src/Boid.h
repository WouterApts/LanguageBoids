//
// Created by wouter on 17-2-2024.
//

#ifndef THESIS_BOID_H
#define THESIS_BOID_H

#include <future>
#include <memory>

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>

#include "Obstacles.h"
#include "World.h"
#include "LanguageManager.h"
#include "SimulationConfig.h"
#include "Terrain.h"

class Boid {
private:
    std::shared_ptr<sf::Texture> p_texture;

public:

    Boid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
         const std::shared_ptr<SimulationConfig>& config,
         float perception_radius,
         float interaction_radius,
         float separation_radius,
         float collision_radius);

    Boid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
         const std::shared_ptr<SimulationConfig>& config);

    virtual ~Boid() = default;

    sf::Sprite sprite;

    Eigen::Vector2f pos;
    Eigen::Vector2f vel;
    Eigen::Vector2f acc;

    std::shared_ptr<SimulationConfig> config;
    float perception_radius;
    float interaction_radius;
    float separation_radius;
    float collision_radius;
    float min_speed;
    float max_speed;

    int spatial_key;

    void UpdateSprite();
    void SetPosition(Eigen::Vector2f position);
    void SetVelocity(Eigen::Vector2f velocity);
    void SetAcceleration(Eigen::Vector2f acceleration);
    void SetMinMaxSpeed(float min, float max);
    void SetDefaultMinMaxSpeed();

    void UpdatePosition(const sf::Time &delta_time);
    void UpdateVelocity(const std::vector<std::shared_ptr<Obstacle>> &obstacles, const sf::Time &delta_time);
    Eigen::Vector2f AvoidBorders(float width, float height) const;

};


class KeyBoid : public Boid {
public:
    int language_key;
    float language_satisfaction;

    KeyBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
            const std::shared_ptr<SimulationConfig> &config,
            int language_key,
            float perception_radius,
            float interaction_radius,
            float separation_radius,
            float collision_radius);

    KeyBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, const std::shared_ptr<SimulationConfig> &config,
            int language_key);

    void UpdateAcceleration(const std::vector<KeyBoid *> &nearby_boids, World &world);
    Eigen::Vector2f CalcCoherenceAlignmentAcceleration(const std::vector<KeyBoid*> &nearby_boids) const;
    Eigen::Vector2f CalcSeparationAcceleration(const std::vector<KeyBoid*>& nearby_boids) const;
    Eigen::Vector2f CalcAvoidanceAcceleration(const std::vector<KeyBoid*>& nearby_boids) const;

    void SetLanguageKey(int key);

    void UpdateColor(LanguageManager &languageManager);
    void UpdateLanguage(const std::vector<KeyBoid*> &nearby_boids, LanguageManager &language_manager,
                        sf::Time delta_time);

    void UpdateLanguageSatisfaction(const std::vector<KeyBoid *> &perceived_boids,
                                    const std::vector<KeyBoid *> &interacting_boids, LanguageManager &language_manager,
                                    sf::Time delta_time);

    void SetLanguageSatisfaction(float value);
};


class VectorBoid : public Boid {
public:
    VectorBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, const std::shared_ptr<SimulationConfig> &config,
            Eigen::VectorXi language, float language_influence,
            float perception_radius,
            float interaction_radius,
            float separation_radius,
            float collision_radius);

    Eigen::VectorXi language_vector;
    float language_influence;

    void UpdateAcceleration(const std::vector<VectorBoid*>& nearby_boids, World& world);
    Eigen::Vector2f CalcAvoidanceAcceleration(const std::vector<VectorBoid*>& nearby_boids, Eigen::VectorXf language_similarities) const;
    Eigen::Vector2f CalcSeparationAcceleration(const std::vector<VectorBoid*>& nearby_boids) const;
    Eigen::Vector2f CalcCoherenceAlignmentAcceleration(const std::vector<VectorBoid*>& nearby_boids, const Eigen::VectorXf& language_similarities) const;

    void UpdateLanguage(const std::vector<VectorBoid*>& nearby_boids, sf::Time delta_time);
    Eigen::VectorXf CalcLanguageSimilarities(const std::vector<VectorBoid *> &boids) const;
};

#endif //THESIS_BOID_H