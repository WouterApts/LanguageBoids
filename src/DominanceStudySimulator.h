//
// Created by wouter on 5-4-2024.
//

#ifndef DOMINANCESTUDY_H
#define DOMINANCESTUDY_H
#include "Application.h"
#include "Camera.h"
#include "SimulationData.h"
#include "Simulator.h"
#include "State.h"


class DominanceStudySimulator : public State {
public:
    std::shared_ptr<Context> context;
    Camera camera;
    KeySimulationData simulation_data;

    std::unique_ptr<KeySimulator> current_simulation;
    int current_run_nr = 0;
    int current_distrubution_nr = 0;
    std::map<int, int> current_initial_distribution;
    std::vector<int> run_outcomes;
    std::vector<double> run_times;
    std::vector<std::array<int, 2>> run_final_distributions;

    std::map<int, int> spawners_per_language;

    void SetLanguageKeysToOneAndZero();

    DominanceStudySimulator(std::shared_ptr<Context>& context, KeySimulationData& simulation_data, float camera_width, float camera_height);

    void Init() override;

    void SetBoidsSpawnedPerSpawner();

    void ProcessInput() override;

    void SetCurrentInitialDistribition();

    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;
};



#endif //DOMINANCESTUDY_H
