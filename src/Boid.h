//
// Created by wouter on 17-2-2024.
//

#ifndef THESIS_BOID_H
#define THESIS_BOID_H

#include <future>
#include <memory>
#include <set>

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>

#include "Obstacles.h"
#include "LanguageManager.h"
#include "SimulationConfig.h"

struct World;

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


class CompBoid : public Boid {
public:
    int language_key;
    float language_satisfaction;
    int updated_language_key = -1;
    std::shared_ptr<std::map<int, float>> language_status_map;

    CompBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
            const std::shared_ptr<SimulationConfig> &config,
            int language_key,
            float perception_radius,
            float interaction_radius,
            float separation_radius,
            float collision_radius);

    CompBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, const std::shared_ptr<SimulationConfig> &config,
            int language_key);

    // No write functions for multi threading.
    std::pair<int, float> GetUpdatedLanguageAndSatisfaction(const std::vector<CompBoid *> &perceived_boids,
                                                            const std::vector<CompBoid *> &interacting_boids,
                                                            sf::Time delta_time) const;
    Eigen::Vector2f GetUpdatedAcceleration(const std::vector<CompBoid *> &interacting_boids) const;

    // Single thread functions
    void UpdateAcceleration(const std::vector<CompBoid *> &interacting_boids);
    Eigen::Vector2f CalcCoherenceAlignmentAcceleration(const std::vector<CompBoid*> &nearby_boids) const;
    Eigen::Vector2f CalcSeparationAcceleration(const std::vector<CompBoid*>& interacting_boids) const;
    Eigen::Vector2f CalcAvoidanceAcceleration(const std::vector<CompBoid*>& nearby_boids) const;
    void UpdateLanguageSatisfaction(const std::vector<CompBoid *> &perceived_boids,
                                  const std::vector<CompBoid *> &interacting_boids,
                                  sf::Time delta_time);

    void UpdateColor();
    void UpdateLanguage();

    void SetLanguageKey(int key);
    void SetLanguageSatisfaction(float value);

    void SetLanguageStatusMap(const std::shared_ptr<std::map<int, float>> &language_status_map);
};


class EvoBoid : public Boid {
public:
    Eigen::VectorXi language_vector;
    float language_influence;
    float age = 0;

    EvoBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, const std::shared_ptr<SimulationConfig> &config,
            Eigen::VectorXi language_vector, float language_influence,
            float perception_radius,
            float interaction_radius,
            float separation_radius,
            float collision_radius);

    EvoBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, const std::shared_ptr<SimulationConfig> &config,
               Eigen::VectorXi language_vector, float language_influence);

    Eigen::VectorXf CalcLanguageDistances(const std::vector<EvoBoid*> &boids) const;
    Eigen::VectorXf CalcLanguageDistances(const std::vector<std::shared_ptr<EvoBoid>> &boids) const;
    float CalcBehaviourModifier(float language_distance) const;

    // Acceleration
    void UpdateAcceleration(const std::vector<EvoBoid *> &interacting_boids, const Eigen::VectorXf &language_distances);
    Eigen::Vector2f GetUpdatedAcceleration(const std::vector<EvoBoid *> &interacting_boids, const Eigen::VectorXf &language_distances) const;
    Eigen::Vector2f CalcAvoidanceAcceleration(const std::vector<EvoBoid*>& interacting_boids, Eigen::VectorXf language_distances) const;
    Eigen::Vector2f CalcSeparationAcceleration(const std::vector<EvoBoid*>& interacting_boids) const;
    Eigen::Vector2f CalcCoherenceAlignmentAcceleration(const std::vector<EvoBoid*>& interacting_boids, const Eigen::VectorXf& language_similarities) const;

    // Language
    void UpdateLanguageFeatures(const std::vector<EvoBoid *> &interacting_boids,
                                const Eigen::VectorXf& language_distances,
                                const std::vector<EvoBoid *> &perceived_boids,
                                sf::Time delta_time);
    std::set<int> GetUpdatedLanguageFeatures(const std::vector<EvoBoid *> &interacting_boids,
                                         const Eigen::VectorXf &language_distances,
                                         const std::vector<EvoBoid*> &perceived_boids,
                                         sf::Time delta_time);
    void SwitchLanguageFeatures(const std::set<int> & features);
    int CalcMutatedLanguageFeature(sf::Time delta_time) const;
    std::set<int> CalcAdoptedLanguageFeatures(const std::vector<EvoBoid *> &interacting_boids, const Eigen::VectorXf &language_distances, const std::vector<
                                              EvoBoid *> &perceived_boids, sf::Time delta_time);

    // Population dynamics
    void UpdateAge(sf::Time delta_time);
    Eigen::VectorXi GetMostCommonLanguage(const std::vector<EvoBoid *> &boids) const;

    Eigen::Vector2f GetOffspringPos(const World& world);
};

#endif //THESIS_BOID_H