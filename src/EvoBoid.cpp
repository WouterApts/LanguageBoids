//
// Created by wouter on 28-2-2024.
//

#include <random>
#include "boid.h"

EvoBoid::EvoBoid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
                 Eigen::VectorXi language, float language_influence, float perception_radius,
                 float interaction_radius, float avoidance_radius, float collision_radius)
    : Boid(std::move(pos), std::move(vel), std::move(acc), perception_radius, interaction_radius, avoidance_radius, collision_radius),
      language_vector(std::move(language)), language_influence(language_influence) {}

void EvoBoid::UpdateAcceleration(const std::vector<EvoBoid*>& nearby_boids, World& world) {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    if (!nearby_boids.empty()) {
        const Eigen::VectorXf& language_similarities = CalcLanguageSimilarities(nearby_boids);
        //Coherence & Alignment
        acceleration += CalcCoherenceAlignmentAcceleration(nearby_boids, language_similarities);
        //Avoidance
        acceleration += CalcAvoidanceAcceleration(nearby_boids, language_similarities);
        //Separation
        acceleration += CalcSeparationAcceleration(nearby_boids);
    }

    //Avoid World borders
    acceleration = acceleration + AvoidBorders(world.width, world.height);

    SetAcceleration(acceleration);
}

Eigen::Vector2f EvoBoid::CalcCoherenceAlignmentAcceleration(const std::vector<EvoBoid*> &nearby_boids,
                                                         const Eigen::VectorXf &language_similarities) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    float total_weight = 0;

    // Calculate average position and velocity of neighbouring boids, using the language similarity as weight.
    Eigen::Vector2f avg_pos = Eigen::Vector2f::Zero();
    Eigen::Vector2f avg_vel = Eigen::Vector2f::Zero();
    for (size_t i = 0; i < nearby_boids.size(); ++i) {
        if (language_similarities(i) > 0) {
            total_weight += language_similarities(i);
            avg_pos += nearby_boids[i]->pos * language_similarities(i);
            avg_vel += nearby_boids[i]->vel * language_similarities(i);
        }
    }

    if (total_weight > 0) {
        avg_pos = avg_pos / total_weight;
        avg_vel = avg_vel / total_weight;

        // COHERENCE
        Eigen::Vector2f pos_difference = avg_pos - this->pos;
        acceleration = pos_difference.normalized() * COHERENCE_FACTOR * max_speed;

        // ALIGNMENT
        Eigen::Vector2f vel_difference = (avg_vel - this->vel);
        acceleration += vel_difference.normalized() * ALIGNMENT_FACTOR * max_speed;
    }

    return acceleration;
}

Eigen::Vector2f EvoBoid::CalcSeparationAcceleration(const std::vector<EvoBoid*>& nearby_boids) const {

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

Eigen::Vector2f EvoBoid::CalcAvoidanceAcceleration(const std::vector<EvoBoid*>& nearby_boids, Eigen::VectorXf language_similarities) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    const float squared_perception_radius = interaction_radius * interaction_radius;

    for (size_t i = 0; i < nearby_boids.size(); ++i) {
        if (language_similarities(i) < 0) {
            Eigen::Vector2f pos_difference = nearby_boids[i]->pos - this->pos;
            float squared_distance = pos_difference.squaredNorm();
            float strength = (squared_perception_radius - squared_distance) / squared_perception_radius;
            acceleration -= pos_difference.normalized() * max_speed * AVOIDANCE_FACTOR * (strength);
        }
    }
    return acceleration;
}

Eigen::VectorXf EvoBoid::CalcLanguageSimilarities(const std::vector<EvoBoid*> &boids) const {
    const size_t num_boids = boids.size();
    Eigen::VectorXf similarities(num_boids);

    for (Eigen::Index i = 0; i < num_boids; ++i) {
        const Eigen::VectorXi& other_language = boids[i]->language_vector;
        float distance = (other_language - this->language_vector).cast<float>().squaredNorm();
        similarities[i] = (0.5f - (distance / LANGUAGE_MAX_DIFF)) * 2.f; // [-1, 1]
    }
    return similarities;
}

void EvoBoid::UpdateLanguage(const std::vector<EvoBoid*>& boids, sf::Time delta_time) {

    if (boids.empty()) {
        return;
    }

    Eigen::VectorXf avg_language = Eigen::VectorXf::Zero(LANGUAGE_SIZE);
    float total_weight = 0;
    for (const auto& boid : boids) {
        total_weight += boid->language_influence;
        avg_language += boid->language_vector.cast<float>() * boid->language_influence;
    }
    if (total_weight > 0) {
        avg_language /= total_weight;
    }

    // Calculate mutation_chance for each bit in the boid's language
    Eigen::VectorXf mutation_chance = (language_vector.cast<float>() - avg_language).cwiseAbs()
                                      * LANGUAGE_MUTATION_RATE * delta_time.asSeconds();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);

    // Flip the bits of the boid's language where mutation occurs
    for (int i = 0; i < language_vector.size(); ++i) {
        if (dis(gen) < mutation_chance(i)) {
            language_vector(i) = (language_vector(i) == 0) ? 1 : 0;
        }
    }
    //TODO: add random mutation
}