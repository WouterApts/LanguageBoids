//
// Created by wouter on 28-3-2024.
//

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

struct SimulationConfig {
    // Default: Flocking
    float COHESION_FACTOR = 0.5f;
    float ALIGNMENT_FACTOR = 0.05f;
    float SEPARATION_FACTOR = 5.f;
    float AVOIDANCE_FACTOR = 0.5f;
    float MAX_SPEED = 100;
    float MIN_SPEED = 50;
    float PERCEPTION_RADIUS = 500;
    float INTERACTION_RADIUS = 250;
    float SEPARATION_RADIUS = 100;

    // Default: Collision Physics
    float BOID_COLLISION_RADIUS = 10;
    float RESTITUTION_COEFFICIENT = 1;

    // Default: Analysis
    int ANALYSIS_LOG_INTERVAL = 0;
    int POSITION_LOG_INTERVAL = 0;

    // Language Key Simulation
    float a_COEFFICIENT = 1.3f;
    float CONVERSION_RATE = 1.f;

    // Language Vector Simulation
    int LANGUAGE_SIZE = 50;
    float MUTATION_RATE = 0.01f;
    float MIN_INTERACTION_RATE = 0.25f;
    float MIN_ADOPTION_RATE = 0.25f;
    int BOID_LIFE_STEPS = 100;
    float BETA = 5;
    float KAPPA = 0.5;

    // Language Dominance Simulation
    int TIME_STEPS_PER_RUN = 5000;
    int RUNS_PER_FRACTION = 50;
    int FRACTIONS = 50;
    int TOTAL_BOIDS = 1000;

    // Multi-Threading (Experimental)
    bool MULTI_THREADING = 1;
};

#endif //CONFIGURATION_H
