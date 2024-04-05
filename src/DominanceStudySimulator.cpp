//
// Created by wouter on 5-4-2024.
//

#include "DominanceStudySimulator.h"

#include <iostream>
#include <set>


DominanceStudySimulator::DominanceStudySimulator(std::shared_ptr<Context> &context, KeySimulationData &simulation_data, float camera_width, float camera_height)
    : context(context), simulation_data(simulation_data), camera(sf::Vector2f(simulation_data.world.width/2, simulation_data.world.height/2), camera_width, camera_height) {

    // set all language_keys to 0 and 1
    SetLanguageKeysToOneAndZero();

    for (auto& spawner: simulation_data.boid_spawners) {
        spawners_per_language[spawner->language_key]++;
    }
}

void DominanceStudySimulator::Init() {
    SetCurrentInitialDistribition();
    SetBoidsSpawnedPerSpawner();
}

void DominanceStudySimulator::ProcessInput() {
    if (current_simulation) {
        current_simulation->ProcessInput();
    }
}

void DominanceStudySimulator::Update(sf::Time deltaTime) {
    if (current_distrubution_nr >= simulation_data.config->DISTRIBUTIONS) {
        // Display study compelte/exit screen;
    }
    //Check if simulation is running.
    else if (!current_simulation) {
        // Start new run of current distribution
        current_simulation = std::make_unique<KeySimulator>(context, simulation_data, camera.default_width, camera.default_height);
        current_simulation->camera = camera;
        current_simulation->Init();
    }
    else {
        current_simulation->Update(deltaTime);

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
                // Log data to file
                //TODO: write distribution data to file (named after simulation file name)

                current_distrubution_nr++;
                if (current_distrubution_nr < simulation_data.config->DISTRIBUTIONS) {
                    // Setup new distribution
                    SetCurrentInitialDistribition();
                    SetBoidsSpawnedPerSpawner();
                    current_run_nr = 0;
                }
            }
        }
    }
}

void DominanceStudySimulator::Pause() {
}

void DominanceStudySimulator::Draw() {
    if (current_simulation) {
        current_simulation->Draw();
    } else {
        context->window->clear(sf::Color::Black);
        sf::CircleShape circle;
        circle.setPosition(100,100);
        circle.setRadius(100);
        context->window->draw(circle);
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

