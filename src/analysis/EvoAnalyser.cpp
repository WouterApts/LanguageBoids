//
// Created by wouter on 03/06/2024.
//

#include "EvoAnalyser.h"

#include <fstream>
#include <iostream>

#include "boid.h"

EvoAnalyser::EvoAnalyser(std::vector<std::shared_ptr<EvoBoid>> &boids) : ref_boids(boids) {
}

void EvoAnalyser::SaveMetricsToCSV(const std::string &filename, sf::Time delta_time) {
    static sf::Time time_since_lastlog;
    time_since_lastlog += delta_time;

    if (time_since_lastlog >= log_time_interval && log_time_interval > sf::seconds(0)) {
        time_since_lastlog = sf::seconds(0);

        // Save Evolution Boid positions and languaeg vector to file
        std::ofstream file(filename, std::ios_base::app);
        if (!file.is_open()) {
            std::cerr << "ERROR: cannot open file " << filename << std::endl;
            return;
        }
        for (auto boid : ref_boids) {
            file << "(" << boid->pos.x() << ", " << boid->pos.y() << ") ";
            file << "[";
            for (size_t i = 0; i < boid->language_vector.size(); ++i) {
                file << boid->language_vector[i];
                if (i < boid->language_vector.size() - 1) {
                    file << ", ";
                }
            }
            file << "]\n";
        }
        file << "\n";
        file.close();
        std::cout << "Boid data succesfully saved to: " << filename << std::endl;
    }
}

void EvoAnalyser::SetLogTimeInterval(sf::Time interval) {
    log_time_interval = interval;
}
