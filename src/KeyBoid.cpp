//
// Created by wouter on 28-2-2024.
//

#include <iostream>
#include <random>

#include "Boid.h"
#include "Utility.h"

KeyBoid::KeyBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, const std::shared_ptr<SimulationConfig> &config,
                 int language_key, float perception_radius, float interaction_radius, float separation_radius, float collision_radius)
    : Boid(std::move(pos), std::move(vel), std::move(acc), config, perception_radius, interaction_radius, separation_radius, collision_radius),
      language_key(language_key), language_satisfaction(1.f) {
}

KeyBoid::KeyBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, const std::shared_ptr<SimulationConfig> &config,
                 int language_key)
    : Boid(std::move(pos), std::move(vel), std::move(acc), config),
      language_key(language_key), language_satisfaction(1.f) {
}

std::pair<int, float> KeyBoid::GetUpdatedLanguageAndSatisfaction(const std::vector<KeyBoid *> &perceived_boids,
                                              const std::vector<KeyBoid *> &interacting_boids,
                                              sf::Time delta_time) const {
    // Calculate language status based on the boids languages within the perception range.
    std::map<int, float> language_status;
    for (auto& boid : perceived_boids) {
        language_status[boid->language_key] += 1;
    }

    // Calculate the proportion of language speakers based on boids within the interaction range.
    std::map<int, float> language_count;
    for(auto& boid : interacting_boids) {
        language_count[boid->language_key] += 1;
    }

    // Calculate the language influence as (s * x^a)
    std::map<int, float> language_influence;
    float total_influence_val = 0.f;
    for (auto& count : language_count) {
        int key = count.first;
        float influence = std::pow(count.second / static_cast<float>(interacting_boids.size()), config->a_COEFFICIENT) *
                          (language_status[key] / static_cast<float>(perceived_boids.size()));
        total_influence_val += influence;
        language_influence[key] = influence;
    }

    // Based on the language influence, sample r to check whether influence of current language increases or decreases.
    float satisfaction = 0;
    if (float r = GetRandomFloatBetween(0, total_influence_val); r <= language_influence[this->language_key]) {
        // Increase current language satisfaction
        satisfaction = this->language_satisfaction + config->INFLUENCE_RATE * delta_time.asSeconds();
    } else {
        // Decrease current language satisfaction
        satisfaction = this->language_satisfaction - config->INFLUENCE_RATE * delta_time.asSeconds();
    }

    // change language if current influence goes below zero
    int language = this->language_key;
    if (this->language_satisfaction <= 0) {
        satisfaction = 1;
        float max_influence = -1.0f;
        // Get language with maximum influence
        for (const auto&[key, influence] : language_influence) {
            if (influence > max_influence) {
                language = key;
                max_influence = influence;
            }
        }
    }
    return {language, satisfaction};
}

void KeyBoid::UpdateLanguageSatisfaction(const std::vector<KeyBoid *>& perceived_boids,
                                       const std::vector<KeyBoid *>& interacting_boids,
                                       sf::Time delta_time) {

    // Calculate language status based on the boids languages within the perception range.
    std::map<int, float> language_status;
    for (auto& boid : perceived_boids) {
        language_status[boid->language_key] += 1;
    }

    // Calculate the proportion of language speakers based on boids within the interaction range.
    std::map<int, float> language_proportion;
    for(auto& boid : interacting_boids) {
        language_proportion[boid->language_key] += 1;
    }

    // Calculate the language influence as (s * x^a)
    std::map<int, float> language_influence;
    float total_influence_val = 0.f;
    for (auto& language : language_proportion) {
        int key = language.first;
        float influence = std::pow(language.second / static_cast<float>(interacting_boids.size()), config->a_COEFFICIENT) *
                          (language_status[key] / static_cast<float>(perceived_boids.size()));
        total_influence_val += influence;
        language_influence[key] = influence;
    }

    // Based on the language influence, sample r to check whether influence of current language increases or decreases.
    if (float r = GetRandomFloatBetween(0, total_influence_val); r <= language_influence[this->language_key]) {
        // Increase current language satisfaction
        SetLanguageSatisfaction(this->language_satisfaction + config->INFLUENCE_RATE * delta_time.asSeconds());
    } else {
        // Decrease current language satisfaction
        SetLanguageSatisfaction(this->language_satisfaction - config->INFLUENCE_RATE * delta_time.asSeconds());
    }

    // change language if current influence goes below zero
    if (this->language_satisfaction <= 0) {
        float max_influence = -1.0f;
        // Get language with maximum influence
        for (const auto&[key, influence] : language_influence) {
            if (influence > max_influence) {
                updated_language_key = key;
                max_influence = influence;
            }
        }
    }
}

void KeyBoid::UpdateLanguage() {
    if (updated_language_key != -1) {
        SetLanguageKey(updated_language_key);
        SetLanguageSatisfaction(1.f);

        //Reset new_language_key
        updated_language_key = -1;
    }
}

void KeyBoid::SetLanguageSatisfaction(float value) {
    this->language_satisfaction = std::min(1.f, value);
}

Eigen::Vector2f KeyBoid::GetUpdatedAcceleration(const std::vector<KeyBoid*>& interacting_boids) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    if (!interacting_boids.empty()) {
        //Coherence & Alignment
        acceleration += CalcCoherenceAlignmentAcceleration(interacting_boids);
        //Avoidance
        acceleration += CalcAvoidanceAcceleration(interacting_boids);
        //Separation
        acceleration += CalcSeparationAcceleration(interacting_boids);
    }
    return acceleration;
}

void KeyBoid::UpdateAcceleration(const std::vector<KeyBoid *>& interacting_boids) {
    Eigen::Vector2f acceleration = GetUpdatedAcceleration(interacting_boids);
    SetAcceleration(acceleration);
}


Eigen::Vector2f KeyBoid::CalcCoherenceAlignmentAcceleration(const std::vector<KeyBoid*> &nearby_boids) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    float similar_boids = 0;

    // Calculate average position and velocity of neighbouring boids, using the language similarity as weight.
    Eigen::Vector2f avg_pos = Eigen::Vector2f::Zero();
    Eigen::Vector2f avg_vel = Eigen::Vector2f::Zero();
    for (auto& boid : nearby_boids) {
        if (boid->language_key == this->language_key) {
            avg_pos += boid->pos;
            avg_vel += boid->vel;
            similar_boids++;
        }
    }

    // Account for own position and velocity.
    // avg_pos += this->pos;
    // avg_vel += this->vel;
    // similar_boids += 1;

    if (similar_boids > 1) {
        avg_pos = avg_pos / similar_boids;
        avg_vel = avg_vel / similar_boids;

        // COHERENCE
        Eigen::Vector2f pos_difference = avg_pos - this->pos;
        acceleration = pos_difference.normalized() * config->COHERENCE_FACTOR * max_speed;

        // ALIGNMENT
        Eigen::Vector2f vel_difference = (avg_vel - this->vel);
        acceleration += vel_difference.normalized() * config->ALIGNMENT_FACTOR * max_speed;
    }

    return acceleration;
}


Eigen::Vector2f KeyBoid::CalcSeparationAcceleration(const std::vector<KeyBoid*>& interacting_boids) const {

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


Eigen::Vector2f KeyBoid::CalcAvoidanceAcceleration(const std::vector<KeyBoid*>& nearby_boids) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    const float squared_interaction_radius = interaction_radius * interaction_radius;

    for (auto& boid : nearby_boids) {
        if (boid->language_key != this->language_key) {
            Eigen::Vector2f pos_difference = boid->pos - this->pos;
            float squared_distance = pos_difference.squaredNorm();
            float strength = std::pow((squared_interaction_radius - squared_distance) / squared_interaction_radius, 2);
            acceleration -= pos_difference.normalized() * max_speed * config->AVOIDANCE_FACTOR * (strength);
        }
    }
    return acceleration;
}

void KeyBoid::SetLanguageKey(int key) {
    this->language_key = key;
    UpdateColor();
}

void KeyBoid::UpdateColor() {
    this->sprite.setColor(LanguageManager::GetLanguageColor(language_key));
}