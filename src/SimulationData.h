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
    CompSimulation,
    EvoSimulation,
    CompStudy
};

class SimulationData {
public:
    SimulationType type = CompSimulation;
    World world;
    std::shared_ptr<SimulationConfig> config;
    std::vector<std::shared_ptr<BoidSpawner>> boid_spawners;

    SimulationData(SimulationType type, World world, const std::shared_ptr<SimulationConfig>& config) : type(type), world(std::move(world)), config(config) {};
    SimulationData() {
        world = World();
        config = std::make_shared<SimulationConfig>();
    }
};

class KeySimulationData : public SimulationData {
public:
    std::vector<std::shared_ptr<CompBoidSpawner>> boid_spawners;
    KeySimulationData(SimulationType type, World world, const std::shared_ptr<SimulationConfig> &config) :  SimulationData(type, std::move(world), config) {};
};

class VectorSimulationData : public SimulationData {
public:
    std::vector<std::shared_ptr<EvoBoidSpawner>> boid_spawners;
    VectorSimulationData(SimulationType type, World world, const std::shared_ptr<SimulationConfig> &config) :  SimulationData(type, std::move(world), config) {};
};
#endif //SIMULATIONDATA_H
