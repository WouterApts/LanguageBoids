//
// Created by wouter on 28-2-2024.
//

#include <iostream>
#include <random>
#include <set>

#include "boid.h"
#include "World.h"
#include "Utility.h"

VectorBoid::VectorBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, const std::shared_ptr<SimulationConfig> &config,
                 Eigen::VectorXi language_vector, float language_influence, float perception_radius,
                 float interaction_radius, float avoidance_radius, float collision_radius)
    : Boid(std::move(pos), std::move(vel), std::move(acc), config, perception_radius, interaction_radius, avoidance_radius, collision_radius),
      language_vector(std::move(language_vector)), language_influence(language_influence) {}

VectorBoid::VectorBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
    const std::shared_ptr<SimulationConfig> &config, Eigen::VectorXi language_vector, float language_influence)
    : Boid(std::move(pos), std::move(vel), std::move(acc), config),
      language_vector(std::move(language_vector)), language_influence(language_influence) {}


Eigen::Vector2f VectorBoid::GetUpdatedAcceleration(const std::vector<VectorBoid*> &interacting_boids, const Eigen::VectorXf& language_distances) const {
    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    if (!interacting_boids.empty()) {
        //Coherence & Alignment
        acceleration += CalcCoherenceAlignmentAcceleration(interacting_boids, language_distances);
        //Avoidance
        acceleration += CalcAvoidanceAcceleration(interacting_boids, language_distances);
        //Separation
        acceleration += CalcSeparationAcceleration(interacting_boids);
    }
    return acceleration;
}

void VectorBoid::UpdateAcceleration(const std::vector<VectorBoid*>& interacting_boids, const Eigen::VectorXf& language_distances) {
    Eigen::Vector2f acceleration = GetUpdatedAcceleration(interacting_boids, language_distances);
    SetAcceleration(acceleration);
}

void VectorBoid::UpdateLanguageFeatures(const std::vector<VectorBoid *> &interacting_boids,
                                        const Eigen::VectorXf& language_distances,
                                        const std::vector<VectorBoid *> &perceived_boids,
                                        sf::Time delta_time) {
    auto features = GetUpdatedLanguageFeatures(interacting_boids, language_distances, perceived_boids, delta_time);
    SwitchLanguageFeatures(features);
}

float VectorBoid::CalcBehaviourModifier(const float language_distance) const {
    float behaviour_modifier;
    float n = (language_distance - 0.5f)*2;
    if (language_distance >= 0.5) {
        behaviour_modifier = (static_cast<float>(std::tanh((2*n-1)*std::numbers::pi)) + 1) / 2;
    } else {
        behaviour_modifier = (static_cast<float>(std::tanh((-2*n-1)*std::numbers::pi)) + 1) / 2;
    }
    return behaviour_modifier;
}

Eigen::Vector2f VectorBoid::CalcCoherenceAlignmentAcceleration(const std::vector<VectorBoid*> &interacting_boids,
                                                               const Eigen::VectorXf &language_distances) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    float total_modifier = 0;

    // Calculate average position and velocity of neighbouring boids, using the language similarity as weight.
    Eigen::Vector2f avg_pos = Eigen::Vector2f::Zero();
    Eigen::Vector2f avg_vel = Eigen::Vector2f::Zero();
    auto number_of_boids = interacting_boids.size();
    for (size_t i = 0; i < number_of_boids; ++i) {
        // Check if interacting boid has a similat language
        const float& language_distance = language_distances(i);
        if (language_distance <= 0.5) {
            float modifier = CalcBehaviourModifier(language_distance);
            total_modifier += modifier;
            avg_pos += interacting_boids[i]->pos * modifier;
            avg_vel += interacting_boids[i]->vel * modifier;
        }
    }

    // account for own velocity and position (with modifier 1, logically)
    avg_pos += this->pos;
    avg_vel += this->vel;
    total_modifier += 1;

    // normalize all modifier weights by deviding with total_modifier weight.
    if (total_modifier > 0) {
        avg_pos = avg_pos / total_modifier;
        avg_vel = avg_vel / total_modifier;

        // COHERENCE
        Eigen::Vector2f pos_difference = avg_pos - this->pos;
        acceleration = pos_difference.normalized() * config->COHERENCE_FACTOR * max_speed;

        // ALIGNMENT
        Eigen::Vector2f vel_difference = avg_vel - this->vel;
        acceleration += vel_difference.normalized() * config->ALIGNMENT_FACTOR * max_speed;
    }

    return acceleration;
}


Eigen::Vector2f VectorBoid::CalcSeparationAcceleration(const std::vector<VectorBoid*>& interacting_boids) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    float squared_separation_radius = separation_radius * separation_radius;

    for (auto & boid : interacting_boids) {
        Eigen::Vector2f pos_difference = boid->pos - this->pos;
        float squared_distance = pos_difference.squaredNorm();
        if (squared_distance <= squared_separation_radius) {
            float strength = std::pow((squared_separation_radius - squared_distance) / squared_separation_radius, 2);
            acceleration -= pos_difference.normalized() * max_speed * config->SEPARATION_FACTOR * (strength);
        }
    }

    return acceleration;
}

Eigen::Vector2f VectorBoid::CalcAvoidanceAcceleration(const std::vector<VectorBoid*>& interacting_boids, Eigen::VectorXf language_distances) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    const float squared_interaction_radius = interaction_radius * interaction_radius;

    for (size_t i = 0; i < interacting_boids.size(); ++i) {
        const float& language_distance = language_distances(i);
        if (language_distance > 0.5) {
            float modifier = CalcBehaviourModifier(language_distance);
            Eigen::Vector2f pos_difference = (interacting_boids[i]->pos - this->pos) * modifier;
            float squared_distance = pos_difference.squaredNorm();
            float strength = std::pow((squared_interaction_radius - squared_distance) / squared_interaction_radius, 2);
            acceleration -= pos_difference.normalized() * max_speed * config->AVOIDANCE_FACTOR * (strength);
        }
    }
    return acceleration;
}

// Use Manhattan distance to calculate distance between vectors.
Eigen::VectorXf VectorBoid::CalcLanguageDistances(const std::vector<VectorBoid*> &boids) const {
    const size_t num_boids = boids.size();
    Eigen::VectorXf distances(num_boids);

    for (Eigen::Index i = 0; i < num_boids; ++i) {
        const Eigen::VectorXi& other_language = boids[i]->language_vector;
        Eigen::VectorXf differences = (other_language - this->language_vector).cast<float>();
        float language_distance = differences.cwiseAbs().sum() / static_cast<float>(config->LANGUAGE_SIZE);
        distances[i] = language_distance;
    }
    return distances;
}

// Use Manhattan distance to calculate distance between vectors.
Eigen::VectorXf VectorBoid::CalcLanguageDistances(const std::vector<std::shared_ptr<VectorBoid>> &boids) const {
    const size_t num_boids = boids.size();
    Eigen::VectorXf distances(num_boids);

    for (Eigen::Index i = 0; i < num_boids; ++i) {
        const Eigen::VectorXi& other_language = boids[i]->language_vector;
        Eigen::VectorXf differences = (other_language - this->language_vector).cast<float>();
        float language_distance = differences.cwiseAbs().sum() / static_cast<float>(config->LANGUAGE_SIZE);
        distances[i] = language_distance;
    }
    return distances;
}

int VectorBoid::CalcMutatedLanguageFeature(sf::Time delta_time) const {
    auto r = GetRandomFloatBetween(0,1);
    int f_index = -1;
    if (r < config->MUTATION_RATE * delta_time.asSeconds()) {
        f_index = GetRandomIntBetween(0, config->LANGUAGE_SIZE - 1);
    }
    return f_index;
}

int CalcFeatureVariantOccurences(int variant, int feature_index, const std::vector<VectorBoid*> &perceived_boids) {
    int occurences = 0;
    for (auto& boid : perceived_boids) {
        if (boid->language_vector(feature_index) == variant) occurences += 1;
    }
    return occurences;
}

std::set<int> VectorBoid::CalcAdoptedLanguageFeatures(const std::vector<VectorBoid *> &interacting_boids,
                                                      const Eigen::VectorXf& language_distances,
                                                      const std::vector<VectorBoid *> &perceived_boids,
                                                      sf::Time delta_time) {
    std::set<int> adopted_features;
    int num_boids = interacting_boids.size();
    for (Eigen::Index i = 0; i < num_boids; ++i) {

        auto boid = interacting_boids[i];
        constexpr int beta = 5;
        float interaction_probability = config->MIN_INTERACTION_RATE
                                          + (1-config->MIN_INTERACTION_RATE) * std::pow(10, - beta*language_distances(i));
        // Interaction probability check per boid
        auto r = GetRandomFloatBetween(0,1);
        if (r < interaction_probability * delta_time.asSeconds()) {
            // Choose a random feature of the boid that's being interacted with,
            int f_index = GetRandomIntBetween(0, config->LANGUAGE_SIZE - 1);
            if (this->language_vector(f_index) != boid->language_vector(f_index)) {
                // Calculate the number of times the feature variant occures in the perceived area
                int f_variant_occurences = CalcFeatureVariantOccurences(boid->language_vector(f_index), f_index, perceived_boids);
                float p = config->MIN_ADOPTION_RATE +  std::pow(f_variant_occurences/perceived_boids.size(), -0.5);
                float adoption_probability = std::min(1.f, p);

                // Feature Adoption probability check
                r = GetRandomFloatBetween(0,1);
                if (r < adoption_probability * delta_time.asSeconds()) {
                    adopted_features.insert(f_index);
                }
            }
        }
    }

    return adopted_features;
}

std::set<int> VectorBoid::GetUpdatedLanguageFeatures(const std::vector<VectorBoid *> &interacting_boids,
                                                     const Eigen::VectorXf &language_distances,
                                                     const std::vector<VectorBoid *> &perceived_boids,
                                                     sf::Time delta_time) {

    std::set<int> updated_features;
    if (age <= config->BOID_LIFE_STEPS / 2) {
        // Get adopted features
        updated_features = CalcAdoptedLanguageFeatures(interacting_boids, language_distances, perceived_boids, delta_time);
        // Get mutated feature
        int mutated_feauture = CalcMutatedLanguageFeature(delta_time);
        if (mutated_feauture != -1) updated_features.insert(mutated_feauture);
    }
    return updated_features;
}

void VectorBoid::SwitchLanguageFeatures(const std::set<int> &features) {
    for (int f_index : features) {
        language_vector(f_index) = !language_vector(f_index);
    }
}

void VectorBoid::UpdateAge(sf::Time delta_time) {
    age += delta_time.asSeconds();
}

Eigen::VectorXi VectorBoid::GetMostCommonLanguage(const std::vector<VectorBoid *> &boids) const {

    std::unordered_map<Eigen::VectorXi, int, vectorXiHash, vectorXiEqual> language_counts;

    // Count occurrences of each language vector
    for (auto& boid : boids) {
        language_counts[boid->language_vector] += 1;
        // if ((boid->pos - this->pos).norm() <= boid->separation_radius) {
        //     language_counts[boid->language_vector] += 1;
        // }
    }

    // Add own language
    language_counts[this->language_vector] += 1;

    // Find the language vector with the highest count
    Eigen::VectorXi most_common_language;
    int max_count = 0;
    for (const auto& pair : language_counts) {
        if (pair.second > max_count) {
            max_count = pair.second;
            most_common_language = pair.first;
        }
    }
    return most_common_language;
}

Eigen::Vector2f VectorBoid::GetOffspringPos(const World& world) {

    float angle = GetRandomFloatBetween(0, 2*std::numbers::pi);
    float length = GetRandomFloatBetween(0, this->collision_radius);
    float x = this->pos.x() + std::cos(angle) * length;
    float y = this->pos.y() + std::sin(angle) * length;

    //Keep spawn position within world border (including buffer)
    float buffer = this->collision_radius;
    x = std::max(std::min(x, world.width - buffer), buffer);
    y = std::max(std::min(y, world.height - buffer), buffer);

    return {x, y};
}
