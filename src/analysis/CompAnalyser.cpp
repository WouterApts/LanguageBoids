//
// Created by wouter on 15-3-2024.
//

#include "CompAnalyser.h"
#include <iostream>
#include <fstream>

CompAnalyser::CompAnalyser(const World &world, int cell_size, std::vector<std::shared_ptr<CompBoid>>& boids, LanguageManager& language_manager)
: ref_boids(boids), ref_language_manager(language_manager), cell_size(cell_size),
  n_cells_in_width(std::ceil(world.width / static_cast<float>(cell_size))),
  n_cells_in_height(std::ceil(world.height / static_cast<float>(cell_size))){
}

void CompAnalyser::LogAllMetrics(sf::Time delta_time) {
    static sf::Time time_since_last_bpl_log;
    static sf::Time time_since_last_lpc_log;

    time_since_last_bpl_log += delta_time;
    time_since_last_lpc_log += delta_time;

    if (time_since_last_bpl_log >= bpl_time_interval) {
        LogBoidsPerLanguage();
        time_since_last_bpl_log = sf::seconds(0);
    }

    if (time_since_last_lpc_log >= lpc_time_interval) {
        LogLanguagesPerCell();
        time_since_last_lpc_log = sf::seconds(0);
    }
}

void CompAnalyser::LogBoidsPerLanguage() {
    auto map = std::map<int, int>();
    for (auto& boid : ref_boids) {
        map[boid->language_key] += 1;
    }
    boids_per_language.push_back(std::move(map));
    std::cout << "BPL logging complete" << std::endl;
}


void CompAnalyser::LogLanguagesPerCell() {
    int dim1 = ref_language_manager.GetNumberOfLanguages();
    int dim2 = n_cells_in_width;
    int dim3 = n_cells_in_height;
    Matrix3D matrix(dim1, std::vector(dim2, std::vector(dim3, 0)));
    for (auto& boid : ref_boids) {
        auto x = boid->pos.x();
        auto y = boid->pos.y();
        int key = boid->language_key;
        int x_cell = std::floor(x / static_cast<float>(cell_size));
        int y_cell = std::floor(y / static_cast<float>(cell_size));
        if (x_cell > n_cells_in_width || y_cell > n_cells_in_height || x_cell < 0 || y_cell < 0) {
            throw std::runtime_error("Boid outside of Analysis Grid");
        } else {
            matrix[key][x_cell][y_cell] += 1;
        }
    }
    languages_per_cell.push_back(std::move(matrix));
    std::cout << "LPC logging complete" << std::endl;
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

void CompAnalyser::SaveLanguagesPerCellToCSV(const std::string &filename) {
    if (!languages_per_cell.empty()) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: cannot open file " << filename << std::endl;
            return;
        }

        // CSV Header
        file << "Time Interval: " << static_cast<int>(lpc_time_interval.asSeconds()) << "\n";

        for (const auto& matrix : languages_per_cell) {
            for (int y = 0; y < matrix[0][0].size(); ++y) {
                for (int x = 0; x < matrix[0].size(); ++x) {
                    file << "(";
                    for (int key = 0; key < matrix.size(); ++key) {
                        file << matrix[key][x][y];
                        if (key != matrix.size()-1) file << ",";
                    }
                    file << "),";
                }
                file << "\n";
            }
            file << "\n";
        }

        file.close();
        std::cout << "Languages per Cell Data succesfully saved to " << filename << std::endl;
    }
}

void CompAnalyser::SetBPLTimeInterval(sf::Time interval) {
    bpl_time_interval = interval;
}

void CompAnalyser::SetLPCTimeInterval(sf::Time interval) {
    lpc_time_interval = interval;
}

