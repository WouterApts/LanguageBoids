//
// Created by wouter on 5-4-2024.
//

#include "DominanceStudySimulator.h"

#include <format>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <utility>

#include "Utility.h"
#include "ui/EscapeStateInterface.h"
#include "ui/StudyInterface.h"


DominanceStudySimulator::DominanceStudySimulator(std::shared_ptr<Context> &context, KeySimulationData &simulation_data,
                                                 std::string sim_file_name_without_extension, float camera_width, float camera_height, int starting_distributin_nr)
    : context(context), simulation_data(simulation_data),
      camera(sf::Vector2f(simulation_data.world.width/2, simulation_data.world.height/2), camera_width, camera_height),
      interface_manager(std::make_shared<InterfaceManager>()),
      output_file_path("output/" + sim_file_name_without_extension + "_output.txt"),
      current_distrubution_nr(starting_distributin_nr) {

    // Create the output directory if it doesn't exist
    if (!std::filesystem::exists("output/"))
        std::filesystem::create_directory("output/");

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
    SetupCurrentDistribution(current_distrubution_nr);
}

void DominanceStudySimulator::ShowDisplayDisabledMessage() {
    std::cout << "Disabling Display " << std::endl;
    context->window->clear(sf::Color::Black);
    sf::Text text("Display disabled (Press SPACE to enable)", *ResourceManager::GetFont("arial")); // Create the text object
    text.setPosition(10, 10); // Set position of the text
    text.setFillColor(sf::Color::White); // Set color of the text
    context->window->draw(text); // Draw the text onto the window
    context->window->display();
}

void DominanceStudySimulator::ProcessInput() {
    // if (current_simulation) {
    //     current_simulation->ProcessInput();
    // }

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*context->window);
    auto mouse_interface_pos = context->window->mapPixelToCoords(mouse_pos, context->window->getDefaultView());
    interface_manager->OnMouseEnter(mouse_interface_pos);
    interface_manager->OnMouseLeave(mouse_interface_pos);

    // TODO: Only poll events once in the program!
    sf::Event event{};
    while(context->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            context->window->close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                interface_manager->OnLeftClick(mouse_interface_pos);
            }
        }

        if (IsKeyPressedOnce(sf::Keyboard::Key::Right)) {
            current_simulation.reset(nullptr);
            SetupCurrentDistribution(current_distrubution_nr + 1);
        }

        if (IsKeyPressedOnce(sf::Keyboard::Key::Left)) {
            current_simulation.reset(nullptr);
            SetupCurrentDistribution(current_distrubution_nr - 1);
        }

        if (IsKeyPressedOnce(sf::Keyboard::Space)) {
            display_simulation = !display_simulation;
            if (display_simulation == false) {
                ShowDisplayDisabledMessage();
            }
        }

        if (IsKeyPressedOnce(sf::Keyboard::Escape)) {
            context->state_manager->PopState();
        }
    }
}

void DominanceStudySimulator::SetupCurrentDistribution(int number) {
    current_distrubution_nr = std::min(simulation_data.config->DISTRIBUTIONS, std::max(number, 0));
    CalcInitialDistribitionValues();
    SetBoidsSpawnedPerSpawner();

    // Reset run number to 0
    current_run_nr = 0;

    // Reset Data vectors
    run_outcomes.clear();
    run_final_distributions.clear();
    run_times.clear();
}

void DominanceStudySimulator::SetNextDistribution() {
    if (fast_analysis) {
        // End simulation (Skip to last number) if a one-sided outcome has been found for both languages.
        if (one_sided_outcome_found[0] && one_sided_outcome_found[1]) {
            current_distrubution_nr = simulation_data.config->DISTRIBUTIONS;
        }
        //Skip distrubutions of which the outcome is already known due to the outcome of previous (less unequal) distributions.
        else if ((current_distrubution_nr%2 == 1 && one_sided_outcome_found[0]) || (current_distrubution_nr%2 == 0 && one_sided_outcome_found[1])) {
            current_distrubution_nr++;
        }
    }
    SetupCurrentDistribution(current_distrubution_nr + 1);
}

void DominanceStudySimulator::Update(sf::Time delta_time) {
    if (delta_time < sf::seconds(1/30.f)) { delta_time = sf::seconds(1/30.f); }

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
        current_simulation->Update(delta_time);

        // Update Interface
        study_interface->simulation_time_fld->text.setString("Simulation Time: " + std::to_string(current_simulation->total_simulation_time));

        // Check terminating conditions
        std::map<int, int> distribution = {{0, 0},
                                           {1, 0}};
        for (auto& boid : current_simulation->boids) {
            distribution[boid->language_key]++;
        }

        if ( distribution[0] == 0 || distribution[1] == 0 || current_simulation->total_simulation_time >= simulation_data.config->TIME_STEPS_PER_RUN) {
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

            run_times.push_back(std::min(static_cast<double>(current_simulation->total_simulation_time), static_cast<double>(simulation_data.config->TIME_STEPS_PER_RUN)));

            // Get current system time
            std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm* time_info = std::localtime(&current_time);

            // Print the time in the format HH:MM:SS
            std::cout << "["<<std::put_time(time_info, "%H:%M:%S") << "] Run Complete! Dominating Language: " << dominating_language_key
                      << ", Simulated time steps: " << current_simulation->total_simulation_time << std::endl;

            // Stop current simulation
            current_simulation.reset(nullptr);
            current_run_nr++;

            // Check if all runs for this distribution have been simulated
            if (current_run_nr >= simulation_data.config->RUNS_PER_DISTRIBUTION) {

                // Check if the current initial distrubution had a one-sided dominant outcome.
                if (current_distrubution_nr != 0 && !run_outcomes.empty()) {
                    if (std::ranges::adjacent_find(run_outcomes, std::not_equal_to<>()) == run_outcomes.end() )
                    {
                        auto language_key = run_outcomes[0];
                        one_sided_outcome_found[language_key] = true;
                        std::cout << "one-sided dominant distribution found for language: " << language_key << std::endl;
                    }
                }

                // Log analysis data to the output file
                LogDataToFile(output_file_path, current_distrubution_nr, current_initial_distribution, run_outcomes, run_times, run_final_distributions);

                // Go to next initial distribution
                if (current_distrubution_nr < simulation_data.config->DISTRIBUTIONS) {
                    SetNextDistribution();
                }
            }
        }
    }
}

void DominanceStudySimulator::Pause() {
}

void DominanceStudySimulator::Draw() {
    if (display_simulation) {
        context->window->clear(sf::Color::Black);
        if (current_simulation) {
            current_simulation->DrawWorldAndBoids();
        }
        interface_manager->DrawComponents(context->window.get());
        context->window->display();
    }
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

void DominanceStudySimulator::CalcInitialDistribitionValues() {
    auto total = static_cast<double>(simulation_data.config->TOTAL_BOIDS);
    auto distributions = static_cast<double>(simulation_data.config->DISTRIBUTIONS);
    // zero (both languages have an equal starting population)
    if (current_distrubution_nr == 0) {
        current_initial_distribution[0] = total/2;
        current_initial_distribution[1] = total - current_initial_distribution[0];
    }
    // odd (language 1 is in the majority)
    else if (current_distrubution_nr % 2) {
        current_initial_distribution[1] = std::min(total, total/2 + total * std::ceil(static_cast<float>(current_distrubution_nr)/2.f)/distributions);
        current_initial_distribution[0] = total - current_initial_distribution[1];
    }
    // even (language 0 is in the majority)
    else {
        current_initial_distribution[0] = std::min(total, total/2 + total * std::ceil(static_cast<float>(current_distrubution_nr)/2.f)/distributions);
        current_initial_distribution[1] = total - current_initial_distribution[0];
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

void DominanceStudySimulator::LogDataToFile(const std::string& file_path,
                                            int distribution_nr,
                                            std::map<int, int> current_initial_distribution,
                                            const std::vector<int>& run_outcomes,
                                            const std::vector<double>& run_times,
                                            const std::vector<std::array<int, 2>>& run_final_distributions) {

    std::ofstream logfile(file_path, std::ios::app); // Open file in append mode
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

    std::cout << "Saved Output to:" << file_path << std::endl;
}


//----------------------------------//
//          PREVIEW STATE           //
//----------------------------------//
DominanceStudyPreview::DominanceStudyPreview(const std::shared_ptr<Context> &context, KeySimulationData simulation_data,
                                             float camera_width, float camera_height)
    : interface_manager(std::make_shared<InterfaceManager>()),
      context(context),
      simulation_data(std::move(simulation_data)){

    simulation_preview = std::make_unique<KeySimulator>(this->context, this->simulation_data, camera_width, camera_height);

    // Set camera viewport to take up 3/4 of the screens height, for displaying a preview of the simultion world.
    simulation_preview->camera.view.setViewport(sf::FloatRect(0.f, 0.25f, 1.f, 0.75f));

}

void DominanceStudyPreview::Init() {
    // Initialize interfaces.

}

void DominanceStudyPreview::ProcessInput() {
    sf::Event event{};
    auto mouse_pos = sf::Mouse::getPosition(*context->window);
    auto mouse_interface_pos = context->window->mapPixelToCoords(mouse_pos, context->window->getDefaultView());
    interface_manager->OnMouseEnter(mouse_interface_pos);
    interface_manager->OnMouseLeave(mouse_interface_pos);

    while (context->window->pollEvent(event)) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                interface_manager->OnLeftClick(mouse_interface_pos);
            }
        }

        if (IsKeyPressedOnce(sf::Keyboard::Escape)) {
            context->state_manager->PopState();
        }
    }
}

void DominanceStudyPreview::Update(sf::Time deltaTime) {}

void DominanceStudyPreview::Pause() {}

void DominanceStudyPreview::Draw() {
    if (simulation_preview) {
        simulation_preview->DrawWorldAndBoids();
        simulation_preview->DrawSpawners();
    }
}

void DominanceStudyPreview::Start() {}

