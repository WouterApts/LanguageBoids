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

    //Data logging
    std::vector<int> run_outcomes;
    std::vector<double> run_times;
    std::vector<std::array<int, 2>> run_final_distributions;
    std::string output_file_name;

    // Interface
    std::shared_ptr<StudyInterface> study_interface;
    std::shared_ptr<EscapeStateInterface> escape_interface;
    std::shared_ptr<InterfaceManager> interface_manager;

    DominanceStudySimulator(std::shared_ptr<Context>& context, KeySimulationData& simulation_data, std::string file_name_without_extension,
                            float camera_width, float camera_height, int starting_distribution_nr = 0);

    static void LogDataToFile(const std::string &file_name, int distribution_nr, std::map<int, int> current_initial_distribution, const std::vector<
                              int> &run_outcomes, const std::vector<double> &run_times, const std::vector<std::array<int, 2>> &
                              run_final_distributions);

    void Init() override;
    void ProcessInput() override;

    void SetAndInitializeCurrentDistribution(int number);

    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;

    void SetCurrentInitialDistribition();
    void SetBoidsSpawnedPerSpawner();
    void SetLanguageKeysToOneAndZero();
};



#endif //DOMINANCESTUDY_H
