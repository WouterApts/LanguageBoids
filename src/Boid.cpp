//
// Created by wouter on 17-2-2024.
//

#include <iostream>
#include <utility>
#include <random>
#include <memory>

#include "Config.h"
#include "Boid.h"
#include "Obstacles.h"

Boid::Boid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
           Eigen::VectorXi language, float language_influence,
           float perception_radius, float avoidance_radius, float collision_radius)
    : pos(std::move(pos)), vel(std::move(vel)), acc(std::move(acc)),
      language(std::move(language)), language_influence(language_influence), perception_radius(perception_radius),
      avoidance_radius(avoidance_radius), collision_radius(collision_radius) {

    max_speed = MAX_SPEED;
    spatial_key = -1;

    p_texture = std::make_shared<sf::Texture>();
    p_texture->loadFromFile("images/boid.png");
    sprite.setTexture(*p_texture);
    sprite.setOrigin(p_texture->getSize().x/2.0f, p_texture->getSize().y/2.0f);
    sprite.setPosition(this->pos.x(), this->pos.y());
    sprite.setColor(sf::Color(255, 0, 0));

}

void Boid::UpdateSprite() {
    sprite.setPosition(pos.x(), pos.y());
    auto angle = static_cast<float>(std::atan2(vel.y(), vel.x()) * 180 / std::numbers::pi);
    sprite.setRotation(angle);
}

void Boid::SetPosition(Eigen::Vector2f position) {
    pos = std::move(position);
}

void Boid::SetVelocity(Eigen::Vector2f velocity) {
    if (velocity.squaredNorm() > MAX_SPEED_SQUARED) {
        velocity = velocity.normalized() * MAX_SPEED;
    }
    else if (velocity.squaredNorm() < MIN_SPEED_SQUARED) {
        velocity = velocity.normalized() * MIN_SPEED;
    }
    vel = std::move(velocity);
}

void Boid::SetAcceleration(Eigen::Vector2f acceleration) {
    acc = std::move(acceleration);
}

void Boid::UpdateAcceleration(const std::vector<Boid*>& nearby_boids, World& world) {

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

Eigen::Vector2f Boid::CalcCoherenceAlignmentAcceleration(const std::vector<Boid*> &nearby_boids,
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

Eigen::Vector2f Boid::CalcSeparationAcceleration(std::vector<Boid*> nearby_boids) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();

    for (auto & nearby_boid : nearby_boids) {
        Eigen::Vector2f pos_difference = nearby_boid->pos - this->pos;
        float squared_distance = pos_difference.squaredNorm();
        float squared_avoidance_radius = avoidance_radius * avoidance_radius;
        if (squared_distance <= squared_avoidance_radius) {
            float strength = (squared_avoidance_radius - squared_distance) / squared_avoidance_radius;
            acceleration = acceleration - pos_difference.normalized() * max_speed * SEPARATION_FACTOR * (strength);
        }
    }

    return acceleration;
}

Eigen::Vector2f Boid::CalcAvoidanceAcceleration(const std::vector<Boid*>& nearby_boids, Eigen::VectorXf language_similarities) const {

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    const float squared_perception_radius = perception_radius * perception_radius;

    for (size_t i = 0; i < nearby_boids.size(); ++i) {
        if (language_similarities(i) < 0) {
            Eigen::Vector2f pos_difference = nearby_boids[i]->pos - this->pos;
            float squared_distance = pos_difference.squaredNorm();
            float strength = (squared_perception_radius - squared_distance) / squared_perception_radius;
            acceleration -= pos_difference.normalized() * max_speed * SEPARATION_FACTOR * (strength);
        }
    }
    return acceleration;
}

Eigen::Vector2f Boid::CalcSeparationAccelerationAlt(Eigen::MatrixXf &positions) const {

    if (positions.size() == 0) {
        return Eigen::Vector2f::Zero();
    }

    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    Eigen::Vector2f avg_pos = Eigen::Vector2f::Zero();
    int boids_to_avoid = 0;
    float squared_avoidance_radius = avoidance_radius * avoidance_radius;

    for (Eigen::Index i = 0; i < positions.cols(); ++i) {
        Eigen::Vector2f pos_difference = positions.col(i) - this->pos;
        float squared_distance = pos_difference.squaredNorm();
        if (squared_distance <= squared_avoidance_radius) {
            avg_pos += positions.col(i);
            boids_to_avoid++;
        }
    }

    if (boids_to_avoid != 0) {
        avg_pos = avg_pos / boids_to_avoid;
        Eigen::Vector2f avg_pos_difference = avg_pos - this->pos;
        float strength = (squared_avoidance_radius - avg_pos_difference.squaredNorm()) / squared_avoidance_radius;
        acceleration -= avg_pos_difference.normalized() * max_speed * SEPARATION_FACTOR * (strength);
        return acceleration;
    } else {
        return Eigen::Vector2f::Zero();
    }
}

Eigen::VectorXf Boid::CalcLanguageSimilarities(const std::vector<Boid *> &boids) const {
    const size_t num_boids = boids.size();
    Eigen::VectorXf similarities(num_boids);

    for (Eigen::Index i = 0; i < num_boids; ++i) {
        const Eigen::VectorXi& other_language = boids[i]->language;
        float distance = (other_language - this->language).cast<float>().squaredNorm();
        similarities[i] = (0.5f - (distance / LANGUAGE_MAX_DIFF)) * 2.f; // [-1, 1]
    }
    return similarities;
}

void Boid::UpdateLanguage(const std::vector<Boid*>& boids, sf::Time delta_time) {

    if (boids.empty()) {
        return;
    }

    Eigen::VectorXf avg_language = Eigen::VectorXf::Zero(LANGUAGE_SIZE);
    float total_weight = 0;
    for (const auto& boid : boids) {
        total_weight += boid->language_influence;
        avg_language += boid->language.cast<float>() * boid->language_influence;
    }
    if (total_weight > 0) {
        avg_language /= total_weight;
    }

    // Calculate mutation_chance for each bit in the boid's language
    Eigen::VectorXf mutation_chance = (language.cast<float>() - avg_language).cwiseAbs()
                                      * LANGUAGE_MUTATION_RATE * delta_time.asSeconds();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);

    // Flip the bits of the boid's language where mutation occurs
    for (int i = 0; i < language.size(); ++i) {
        if (dis(gen) < mutation_chance(i)) {
            language(i) = (language(i) == 0) ? 1 : 0;
        }
    }
    //TODO: add random mutation
}

Eigen::Vector2f Boid::AvoidBorders(float width, float height) const {
    constexpr int buffer_zone = 300;
    Eigen::Vector2f acceleration = Eigen::Vector2f::Zero();
    if (pos.x() < buffer_zone) {
        acceleration.x() += MAX_SPEED*2;
    }
    if (pos.x() > width - buffer_zone) {
        acceleration.x() -= MAX_SPEED*2;
    }
    if (pos.y() < buffer_zone) {
        acceleration.y() += MAX_SPEED*2;
    }
    if (pos.y() > height - buffer_zone) {
        acceleration.y() -= MAX_SPEED*2;
    }
    return acceleration;
}

void Boid::UpdateVelocity(const std::vector<std::shared_ptr<Obstacle>> &obstacles, const sf::Time &delta_time) {

    // Handle collisions
    Eigen::Vector2f collision_normal = Eigen::Vector2f::Zero();
    for(auto& obstacle : obstacles) {
        collision_normal += obstacle->CalcCollisionNormal(this);
    }
    if (!collision_normal.isApprox(Eigen::Vector2f::Zero())) {
        float velocity_along_normal = vel.dot(collision_normal);

        // If the boid is moving away from the collision, do nothing
        if (velocity_along_normal > 0) {
            SetVelocity(vel + acc * delta_time.asSeconds());
        } else {
            float impulse_magnitude = -(1 + RESTITUTION_COEFFICIENT) * velocity_along_normal;
            Eigen::Vector2f impulse = collision_normal * impulse_magnitude;
            //Eigen::Vector2f reflection = vel - 2*vel.dot(normal)*normal;
            SetVelocity(vel + impulse); //TODO: use reflection in velocity update
        }
    } else {
        SetVelocity(vel + acc * delta_time.asSeconds());
    }
}

void Boid::UpdatePosition(const sf::Time &delta_time) {
    SetPosition(pos + vel * delta_time.asSeconds());
}

