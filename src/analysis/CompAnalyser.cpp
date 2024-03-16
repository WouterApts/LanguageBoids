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

void CompAnalyser::LogBoidsPerLanguage() {
    auto map = std::map<int, int>();
    for (auto& boid : ref_boids) {
        map[boid->language_key] += 1;
    }
    boids_per_language.push_back(std::move(map));
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
}

void CompAnalyser::SaveBoidPerLanguageToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: cannot open file " << filename << std::endl;
        return;
    }

    // CSV Header
    file << "Timestep, Language, Boids \n";

    int timestep = 0;
    for (const auto& map : boids_per_language) {
        for (const auto& [language_key, boids] : map) {
            file << timestep << "," << language_key << "," << boids << "\n";
        }
        timestep++;
    }

    file.close();
    std::cout << "Boid per Language Data succesfully saved to " << filename << std::endl;
}

void CompAnalyser::SaveLanguagesPerCellToCSV(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: cannot open file " << filename << std::endl;
        return;
    }

    // CSV Header
    file << "Timestep interval: t \n";

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

