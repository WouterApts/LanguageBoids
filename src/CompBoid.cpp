//
// Created by wouter on 28-2-2024.
//

#include <iostream>
#include <random>

#include "Boid.h"
#include "Utility.h"

CompBoid::CompBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc, int language_key,
                   float perception_radius, float interaction_radius, float avoidance_radius, float collision_radius)
    : Boid(std::move(pos), std::move(vel), std::move(acc), perception_radius, interaction_radius, avoidance_radius, collision_radius),
      language_key(language_key), language_satisfaction(1.f) {
}

void CompBoid::UpdateLanguage(const std::vector<CompBoid*>& nearby_boids, LanguageManager& language_manager, sf::Time delta_time) {
    // Calculate prevalence per language in the boid's neighborhood.
    std::unordered_map<int, float> language_prevalence;
    language_prevalence[this->language_key] += 1;
    for (auto& boid : nearby_boids) {
        language_prevalence[boid->language_key] += 1;
    }

    // For each language, calculate chance for boid to convert to it.
    float language_prevalence_sum = 0;
    for (auto& [key, value] : language_prevalence) {
        Language* language = language_manager.GetLanguage(key).get();
        value /= static_cast<float>(nearby_boids.size());
        value *= language->popularity_status;
        language_prevalence_sum += value;
    }

    // Using a random number, choose a language based on the previously calculated chances.
    float r = GetRandomFloatBetween(0, language_prevalence_sum);
    float prev_threshold = 0;
    for (auto it = language_prevalence.begin(); it != language_prevalence.end(); ++it) {
        if (r < language_prevalence[it->first] + prev_threshold) {
            int new_language_key = it->first;
            if (new_language_key != this->language_key) {
                this->language_key = new_language_key;
                this->UpdateColor(language_manager);
            }
            break;  // Exit loop once a new language is selected
        } else {
            prev_threshold += language_prevalence[it->first];
        }
    }
}

void CompBoid::UpdateLanguageSatisfaction(const std::vector<CompBoid *> &perceived_boids,
                                          const std::vector<CompBoid *> &interacting_boids,
                                          LanguageManager& language_manager,
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
        float influence = std::pow(language.second / static_cast<float>(interacting_boids.size()), a_COEFFICIENT) *
                          (language_status[key] / static_cast<float>(perceived_boids.size()));
        total_influence_val += influence;
        language_influence[key] = influence;
    }

    // Based on the language influence, sample r to check whether influence of current language increases or decreases.
    if (float r = GetRandomFloatBetween(0, total_influence_val); r <= language_influence[this->language_key]) {
        // Increase current language satisfaction
        SetLanguageSatisfaction(this->language_satisfaction + INFLUENCE_RATE * delta_time.asSeconds());
    } else {
        // Decrease current language satisfaction
        SetLanguageSatisfaction(this->language_satisfaction - INFLUENCE_RATE * delta_time.asSeconds());
    }

    // change language if current influence goes below zero
    if (this->language_satisfaction <= 0) {
        int max_language_key = -1; // Initialize max_language to an invalid value
        float max_influence = -1.0f;
        // Get language with maximum influence
        for (const auto&[key, influence] : language_influence) {
            if (influence > max_influence) {
                max_language_key = key;
                max_influence = influence;
            }
        }
        SetLanguageKey(max_language_key);
        SetLanguageSatisfaction(1.f);
        UpdateColor(language_manager);
    }
}

void CompBoid::SetLanguageSatisfaction(float value) {
    this->language_satisfaction = std::min(1.f, value);
}

void CompBoid::UpdateAcceleration(const std::vector<CompBoid *>& nearby_boids, World& world) {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    if (!nearby_boids.empty()) {
        //Coherence & Alignment
        acceleration += CalcCoherenceAlignmentAcceleration(nearby_boids);
        //Avoidance
        acceleration += CalcAvoidanceAcceleration(nearby_boids);
        //Separation
        acceleration += CalcSeparationAcceleration(nearby_boids);
    }

    // Avoid World borders
    // acceleration = acceleration + AvoidBorders(world.width, world.height);

    SetAcceleration(acceleration);
}


Eigen::Vector2f CompBoid::CalcCoherenceAlignmentAcceleration(const std::vector<CompBoid*> &nearby_boids) const {

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

    if (similar_boids > 0) {
        avg_pos = avg_pos / similar_boids;
        avg_vel = avg_vel / similar_boids;

        // COHERENCE
        Eigen::Vector2f pos_difference = avg_pos - this->pos;
        acceleration = pos_difference.normalized() * COHERENCE_FACTOR * max_speed;

        // ALIGNMENT
        Eigen::Vector2f vel_difference = (avg_vel - this->vel);
        acceleration += vel_difference.normalized() * ALIGNMENT_FACTOR * max_speed;
    }

    return acceleration;
}


Eigen::Vector2f CompBoid::CalcSeparationAcceleration(const std::vector<CompBoid*>& nearby_boids) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();

    for (auto & nearby_boid : nearby_boids) {
        Eigen::Vector2f pos_difference = nearby_boid->pos - this->pos;
        float squared_distance = pos_difference.squaredNorm();
        float squared_avoidance_radius = separation_radius * separation_radius;
        if (squared_distance <= squared_avoidance_radius) {
            float strength = (squared_avoidance_radius - squared_distance) / squared_avoidance_radius;
            acceleration = acceleration - pos_difference.normalized() * max_speed * SEPARATION_FACTOR * (strength);
        }
    }

    return acceleration;
}


Eigen::Vector2f CompBoid::CalcAvoidanceAcceleration(const std::vector<CompBoid*>& nearby_boids) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    const float squared_perception_radius = interaction_radius * interaction_radius;

    for (auto& boid : nearby_boids) {
        if (boid->language_key != this->language_key) {
            Eigen::Vector2f pos_difference = boid->pos - this->pos;
            float squared_distance = pos_difference.squaredNorm();
            float strength = (squared_perception_radius - squared_distance) / squared_perception_radius;
            acceleration -= pos_difference.normalized() * max_speed * AVOIDANCE_FACTOR * (strength);
        }
    }
    return acceleration;
}

void CompBoid::SetLanguageKey(int key) {
    this->language_key = key;
}

void CompBoid::UpdateColor(LanguageManager& languageManager) {
    try {
        std::shared_ptr<Language> language = languageManager.GetLanguage(language_key);
        this->sprite.setColor(language->color);
    } catch (const std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
}