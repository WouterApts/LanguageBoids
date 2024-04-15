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
#include "ui/EscapeStateInterface.h"
#include "ui/StudyInterface.h"


class DominanceStudySimulator : public State {
public:
    std::shared_ptr<Context> context;
    Camera camera;
    KeySimulationData simulation_data;
    std::map<int, int> spawners_per_language;

    std::unique_ptr<KeySimulator> current_simulation;
    int current_run_nr = 0;
    int current_distrubution_nr = 0;
    std::map<int, int> current_initial_distribution;
    bool fast_analysis = false;

    //Data logging
    std::vector<int> run_outcomes;
    std::vector<double> run_times;
    std::vector<std::array<int, 2>> run_final_distributions;
    std::string output_file_path;

    // Interface
    std::shared_ptr<StudyInterface> study_interface;
    std::shared_ptr<EscapeStateInterface> escape_interface;
    std::shared_ptr<InterfaceManager> interface_manager;
    std::map<int, bool> one_sided_outcome_found;

    DominanceStudySimulator(std::shared_ptr<Context>& context, KeySimulationData& simulation_data, std::string sim_file_name_without_extension,
                            float camera_width, float camera_height, int starting_distribution_nr = 0);

    static void LogDataToFile(const std::string &file_path, int distribution_nr, std::map<int, int> current_initial_distribution, const std::vector<
                              int> &run_outcomes, const std::vector<double> &run_times, const std::vector<std::array<int, 2>> &
                              run_final_distributions);

    void Init() override;
    void ProcessInput() override;

    void SetupCurrentDistribution(int number);

    void SetNextDistribution();

    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;

    void CalcInitialDistribitionValues();
    void SetBoidsSpawnedPerSpawner();
    void SetLanguageKeysToOneAndZero();
};

class DominanceStudyPreview : public State {
public:

    std::string output_file_name;
    KeySimulationData simulation_data;

    std::shared_ptr<Context> context;
    std::shared_ptr<InterfaceManager> interface_manager;

    std::unique_ptr<KeySimulator> simulation_preview;

    DominanceStudyPreview(const std::shared_ptr<Context>& context, KeySimulationData  simulation_data, float camera_width, float camera_height);

    void Init() override;
    void ProcessInput() override;
    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;
};


#endif //DOMINANCESTUDY_H
