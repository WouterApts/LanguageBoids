//
// Created by wouter on 5-4-2024.
//

#include "DominanceStudySimulator.h"

#include <format>
#include <fstream>
#include <iostream>
#include <set>
#include <utility>

#include "Utility.h"
#include "ui/EscapeStateInterface.h"
#include "ui/StudyInterface.h"


DominanceStudySimulator::DominanceStudySimulator(std::shared_ptr<Context> &context, KeySimulationData &simulation_data,
                                                 std::string output_file_name, float camera_width, float camera_height, int starting_distributin_nr)
    : context(context), simulation_data(simulation_data),
      camera(sf::Vector2f(simulation_data.world.width/2, simulation_data.world.height/2), camera_width, camera_height),
      interface_manager(std::make_shared<InterfaceManager>()),
      output_file_name(std::move(output_file_name)),
      current_distrubution_nr(starting_distributin_nr) {

    // set all language_keys to 0 and 1
    SetLanguageKeysToOneAndZero();

    // Calculate the ammount of spawners for each language key
    for (auto& spawner: simulation_data.boid_spawners) {
        spawners_per_language[spawner->language_key]++;
    }
}

void DominanceStudySimulator::Init() {

    // Initialize Interface
    auto escape_interface_pos = sf::Vector2f(context->window->getSize().x/2 - 200, context->window->getSize().y/2 - 100);
    escape_interface = std::make_shared<EscapeStateInterface>(escape_interface_pos, context, "Study Complete.\nExit Simulator?", 20);
    interface_manager->AddComponent(escape_interface);
    escape_interface->Deactivate();

    study_interface = std::make_shared<StudyInterface>(sf::Vector2f(20, 20));
    interface_manager->AddComponent(study_interface);

    // Calculate Distribution and setup boids spanwed for each Spawner
    SetAndInitializeCurrentDistribution(current_distrubution_nr);

    // Reset the output data file
    std::ofstream file(output_file_name);

}

void DominanceStudySimulator::ProcessInput() {
    if (current_simulation) {
        current_simulation->ProcessInput();
    }

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    auto mouse_interface_pos = context->window->mapPixelToCoords(mouse_pos, context->window->getDefaultView());
    interface_manager->OnMouseEnter(mouse_interface_pos);
    interface_manager->OnMouseLeave(mouse_interface_pos);

    sf::Event event{};
    while(context->window->pollEvent(event)) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                interface_manager->OnLeftClick(mouse_interface_pos);
            }
        }
    }

    if (IsKeyPressedOnce(sf::Keyboard::Key::Right)) {
        current_simulation.reset(nullptr);
        SetAndInitializeCurrentDistribution(current_distrubution_nr + 1);
    }

    if (IsKeyPressedOnce(sf::Keyboard::Key::Left)) {
        current_simulation.reset(nullptr);
        SetAndInitializeCurrentDistribution(current_distrubution_nr - 1);
    }
}

void DominanceStudySimulator::SetAndInitializeCurrentDistribution(int number) {
    current_distrubution_nr = std::min(simulation_data.config->DISTRIBUTIONS, std::max(number, 0));
    SetCurrentInitialDistribition();
    SetBoidsSpawnedPerSpawner();

    // Reset run number to 0
    current_run_nr = 0;

    // Reset Data vectors
    run_outcomes.clear();
    run_final_distributions.clear();
    run_times.clear();
}

void DominanceStudySimulator::Update(sf::Time deltaTime) {
    if (deltaTime < sf::seconds(1/30.f)) { deltaTime = sf::seconds(1/30.f); }

    if (current_distrubution_nr >= simulation_data.config->DISTRIBUTIONS) {
        escape_interface->Activate();
        study_interface->Deactivate();
    }

    //Check if simulation is running.
    else if (!current_simulation) {
        // Start new run of current distribution
        current_simulation = std::make_unique<KeySimulator>(context, simulation_data, camera.default_width, camera.default_height);
        current_simulation->camera = camera;
        current_simulation->Init();

        std::string distributionString = std::format("Distribution: {}/{} ({}, {})", current_distrubution_nr, simulation_data.config->DISTRIBUTIONS,
                                                      current_initial_distribution[0], current_initial_distribution[1]);
        study_interface->distribution_fld->text.setString(distributionString);

        // Update run number text
        std::string runNumberString = std::format("Run: {}/{}", current_run_nr, simulation_data.config->RUNS_PER_DISTRIBUTION);
        study_interface->run_fld->text.setString(runNumberString);
    }
    else {
        //Update Simulation
        current_simulation->Update(deltaTime);

        // Update Interface
        study_interface->simulation_time_fld->text.setString("Simulation Time: " + std::to_string(current_simulation->total_simulation_time));

        // Check terminating conditions
        std::map<int, int> distribution = {{0, 0},
                                           {1, 0}};
        for (auto& boid : current_simulation->boids) {
            distribution[boid->language_key]++;
        }

        if ( distribution[0] == 0 || distribution[1] == 0 || current_simulation->total_simulation_time >= simulation_data.config->SECONDS_PER_RUN) {
            // Log the necessary data about the simulations outcome
            int dominating_language_key = -1;
            if (distribution[0] == 0) {
                dominating_language_key = 1;
            } else if (distribution[1] == 0) {
                dominating_language_key = 0;
            }
            run_outcomes.push_back(dominating_language_key);

            std::array final_distribution = {distribution[0], distribution[1]};
            run_final_distributions.push_back(final_distribution);

            run_times.push_back(std::min(static_cast<double>(current_simulation->total_simulation_time), static_cast<double>(simulation_data.config->SECONDS_PER_RUN)));

            std::cout << "logging data " << current_distrubution_nr << ": " << current_initial_distribution[0] << ", " << current_initial_distribution[1] << std::endl;
            std::cout << current_simulation->total_simulation_time << std::endl;

            // Stop current simulation
            current_simulation.reset(nullptr);
            current_run_nr++;

            // Check if all runs for this distribution have been simulated
            if (current_run_nr >= simulation_data.config->RUNS_PER_DISTRIBUTION) {

                //LogDataToFile
                LogDataToFile(output_file_name, current_distrubution_nr, current_initial_distribution, run_outcomes, run_times, run_final_distributions);

                if (current_distrubution_nr < simulation_data.config->DISTRIBUTIONS) {
                    SetAndInitializeCurrentDistribution(current_distrubution_nr + 1);
                }
            }
        }
    }
}

void DominanceStudySimulator::Pause() {
}

void DominanceStudySimulator::Draw() {
    context->window->clear(sf::Color::Black);
    if (current_simulation) {
        current_simulation->DrawWorldAndBoids();
    }
    interface_manager->DrawComponents(context->window.get());
    context->window->display();
}

void DominanceStudySimulator::Start() {
}

void DominanceStudySimulator::SetBoidsSpawnedPerSpawner() {

    int a[2] = {1,1};
    int b[2] = {1,1};;

    for (auto& spawner : simulation_data.boid_spawners) {
        int d = spawners_per_language[spawner->language_key];
        int n = current_initial_distribution[spawner->language_key];
        int q = n / d;
        int r = n % d;
        int c = d - r;

        if ((a[spawner->language_key] * r) < (b[spawner->language_key] * c)) {
            a[spawner->language_key]++;
            spawner->boids_spawned = q;
        } else {
            b[spawner->language_key]++;
            spawner->boids_spawned = q + 1;
        }
    }
}

void DominanceStudySimulator::SetCurrentInitialDistribition() {
    auto total = static_cast<double>(simulation_data.config->TOTAL_BOIDS);
    auto distributions = static_cast<double>(simulation_data.config->DISTRIBUTIONS);
    if (current_distrubution_nr == 0) {
        current_initial_distribution[0] = total/2;
        current_initial_distribution[1] = total - current_initial_distribution[0];
    }
    // odd
    else if (current_distrubution_nr % 2) {
        current_initial_distribution[0] = std::min(total, total/2 + total * std::ceil(static_cast<float>(current_distrubution_nr)/2.f)/distributions);
        current_initial_distribution[1] = total - current_initial_distribution[0];
    }
    // even
    else {
        current_initial_distribution[1] = std::min(total, total/2 + total * std::ceil(static_cast<float>(current_distrubution_nr)/2.f)/distributions);
        current_initial_distribution[0] = total - current_initial_distribution[1];
    }
}

void DominanceStudySimulator::SetLanguageKeysToOneAndZero() {
    std::set<int> keys;
    for (auto& spawner : simulation_data.boid_spawners) {
        keys.insert(spawner->language_key);
    }

    std::map<int, int> mapping;
    int i = 0;
    for (int key : keys) {
        mapping[key] = i;
        i++;
    }

    for (auto& spawner : simulation_data.boid_spawners) {
        spawner->language_key = mapping[spawner->language_key];
    }
}

void DominanceStudySimulator::LogDataToFile(const std::string& file_name,
                                            int distribution_nr,
                                            std::map<int, int> current_initial_distribution,
                                            const std::vector<int>& run_outcomes,
                                            const std::vector<double>& run_times,
                                            const std::vector<std::array<int, 2>>& run_final_distributions) {
    std::ofstream logfile(file_name, std::ios::app); // Open file in append mode
    if (!logfile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Write data to file
    logfile << "Distribution-Number: " << distribution_nr << ", Distribution: (" << current_initial_distribution[0] << ", " << current_initial_distribution[1] << ")\n";

    logfile << "Outcome: [";
    for (int i = 0; i < run_outcomes.size(); ++i) {
        logfile << run_outcomes[i];
        if (i < run_outcomes.size() - 1) logfile << ", ";
    }
    logfile << "]\n";

    logfile << "Time: [";
    for (int i = 0; i < run_times.size(); ++i) {
        logfile << run_times[i];
        if (i < run_times.size() - 1) logfile << ", ";
    }
    logfile << "]\n";

    logfile << "Final-Distribution: [";
    for (int i = 0; i < run_final_distributions.size(); ++i) {
        logfile << "(" << run_final_distributions[i][0] << ", " << run_final_distributions[i][1] << ")";
        if (i < run_final_distributions.size() - 1) logfile << ", ";
    }
    logfile << "]\n";

    logfile << "\n";
    logfile.close();
}

