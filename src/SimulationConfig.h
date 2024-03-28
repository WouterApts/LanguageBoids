//
// Created by wouter on 28-3-2024.
//

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

struct SimulationConfig {
    float COHERENCE_FACTOR = 1.f;
    float ALIGNMENT_FACTOR = 0.3f;
    float SEPARATION_FACTOR = 5.f;
    float AVOIDANCE_FACTOR = 0.3f;

    float MAX_SPEED = 150;
    float MIN_SPEED = 75;

    float PERCEPTION_RADIUS = 500;
    float INTERACTION_RADIUS = 250;
    float SEPARATION_RADIUS = 100;

    // Collision Physics
    float BOID_COLLISION_RADIUS = 10;
    float RESTITUTION_COEFFICIENT = 1;

    // Analysis
    int LANGUAGE_LOG_INTERVAL = 1;
    int POSITION_LOG_INTERVAL = 5;

    // Language Key Simulation
    float a_COEFFICIENT = 1.3f;
    float INFLUENCE_RATE = 10.f;

    // Language Vector Simulation
    int LANGUAGE_SIZE = 30;
    float MUTATION_RATE = 0.01f;

    // Language Dominance Simulation
    int RUNS_PER_DISTRIBUTION = 20;
    int DISTRIBUTIONS = 20;
    int TOTAL_BOIDS = 3000;
};
#endif //CONFIGURATION_H
