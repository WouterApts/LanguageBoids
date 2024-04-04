//
// Created by wouter on 28-3-2024.
//

#ifndef SIMULATIONDATA_H
#define SIMULATIONDATA_H

#include <utility>
#include <vector>

#include "BoidSpawners.h"
#include "SimulationConfig.h"
#include "World.h"

enum SimulationType {
    KeySimulation,
    VectorSimulation,
    DominanceStudy
};

class SimulationData {
public:
    SimulationType type = KeySimulation;
    World world;
    std::shared_ptr<SimulationConfig> config;
    std::vector<std::shared_ptr<BoidSpawner>> boid_spawners;

    SimulationData(World world, const std::shared_ptr<SimulationConfig>& config) : world(std::move(world)), config(config) {};
    SimulationData() {
        world = World();
        config = std::make_shared<SimulationConfig>();
    }
};

class KeySimulationData : public SimulationData {
public:
    std::vector<std::shared_ptr<KeyBoidSpawner>> boid_spawners;
    KeySimulationData(World world, const std::shared_ptr<SimulationConfig> &config) :  SimulationData(std::move(world), config) {};
};

class VectorSimulationData : public SimulationData {};
#endif //SIMULATIONDATA_H
