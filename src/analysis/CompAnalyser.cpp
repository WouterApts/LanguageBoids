//
// Created by wouter on 15-3-2024.
//

#include "CompAnalyser.h"
#include <iostream>
#include <fstream>

CompAnalyser::CompAnalyser(std::vector<std::shared_ptr<CompBoid>>& boids)
: ref_boids(boids) {
}

void CompAnalyser::LogAllMetrics(sf::Time delta_time) {
    static sf::Time time_since_last_bpl_log;
    static sf::Time time_since_last_lpc_log;

    time_since_last_bpl_log += delta_time;
    time_since_last_lpc_log += delta_time;

    if (time_since_last_bpl_log >= bpl_time_interval && bpl_time_interval > sf::seconds(0)) {
        LogBoidsPerLanguage();
        time_since_last_bpl_log = sf::seconds(0);
    }

    if (time_since_last_lpc_log >= ppl_time_interval && ppl_time_interval > sf::seconds(0)) {
        LogBoidPositions();
        time_since_last_lpc_log = sf::seconds(0);
    }
}

void CompAnalyser::LogBoidsPerLanguage() {
    auto map = std::map<int, int>();
    for (auto& boid : ref_boids) {
        map[boid->language_key] += 1;
    }
    boids_per_language.push_back(std::move(map));
    std::cout << "Language logging complete! (Press F5 to save to output file)" << std::endl;
}

void CompAnalyser::LogBoidPositions() {
    std::map<int, std::vector<Eigen::Vector2i>> map;
    for (auto& boid : ref_boids) {
        Eigen::Vector2i pos = boid->pos.cast<int>();
        map[boid->language_key].push_back(pos);
    }
    positions_per_language.push_back(map);
    std::cout << "Position logging logging complete! (Press F5 to save to output file)" << std::endl;
}

void CompAnalyser::SaveBoidPerLanguageToCSV(const std::string& filename) {
    if (!boids_per_language.empty()) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: cannot open file " << filename << std::endl;
            return;
        }

        // CSV Header
        file << "Time Interval: " << static_cast<int>(bpl_time_interval.asSeconds()) << ", Languages: " << boids_per_language[0].size() << " ,Boids \n";

        int timestep = 0;
        for (const auto& map : boids_per_language) {
            for (const auto& [language_key, boids] : map) {
                file << static_cast<int>(timestep * bpl_time_interval.asSeconds()) << "," << language_key << "," << boids << "\n";
            }
            timestep++;
        }

        file.close();
        std::cout << "Boid per Language Data succesfully saved to " << filename << std::endl;
    }
}

void CompAnalyser::SavePositionPerLanguageCSV(const std::string& filename) {
    if (!positions_per_language.empty()) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: cannot open file " << filename << std::endl;
            return;
        }

        // CSV Header
        file << "Time Interval: " << static_cast<int>(ppl_time_interval.asSeconds()) << " ,Language: Positions\n";

        for (const auto& map : positions_per_language) {
            file << "\n";
            for (const auto& [language_key, positions] : map) {
                file << language_key << ": [";
                for (int i = 0; i < positions.size(); i++) {
                    file << "(" << positions[i].x() << "," << positions[i].y() << ")";
                    if (i < positions.size() - 1) {
                        file << ", ";
                    }
                }
                file << "]\n";
            }
        }

        file.close();
        std::cout << "Position per Language Data successfully saved to " << filename << std::endl;
    }
}

void CompAnalyser::SetBPLTimeInterval(sf::Time interval) {
    bpl_time_interval = interval;
}

void CompAnalyser::SetPPLTimeInterval(sf::Time interval) {
    ppl_time_interval = interval;
}

