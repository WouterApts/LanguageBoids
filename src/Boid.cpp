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
#include "RescourceManager.h"

Boid::Boid(Eigen::Vector2f pos, Eigen::Vector2f vel, Eigen::Vector2f acc,
           float perception_radius, float interaction_radius, float avoidance_radius, float collision_radius)
    : pos(std::move(pos)), vel(std::move(vel)), acc(std::move(acc)),
      perception_radius(perception_radius), interaction_radius(interaction_radius), separation_radius(avoidance_radius), collision_radius(collision_radius) {

    max_speed = MAX_SPEED;
    min_speed = MIN_SPEED;
    spatial_key = -1;

    const auto& p_texture = RescourceManager::GetTexture("boid");
    sprite.setTexture(*p_texture);
    sprite.setOrigin(p_texture->getSize().x/2.0f, p_texture->getSize().y/2.0f);
    sprite.setPosition(this->pos.x(), this->pos.y());
    sprite.setColor(sf::Color(255, 0, 0));

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
            SetVelocity(vel + impulse);
        }
    } else {
        SetVelocity(vel + acc * delta_time.asSeconds());
    }
}

void Boid::SetMinMaxSpeed(float min, float max) {
    this->max_speed = max;
    this->min_speed = min;
}

void Boid::SetDefaultMinMaxSpeed() {
    this->max_speed = MAX_SPEED;
    this->min_speed = MIN_SPEED;
}

void Boid::UpdatePosition(const sf::Time &delta_time) {
    SetPosition(pos + vel * delta_time.asSeconds());
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
    if (velocity.norm() > max_speed) {
        velocity = velocity.normalized() * max_speed;
    }
    else if (velocity.norm() < min_speed) {
        velocity = velocity.normalized() * min_speed;
    }
    vel = std::move(velocity);
}

void Boid::SetAcceleration(Eigen::Vector2f acceleration) {
    acc = std::move(acceleration);
}





