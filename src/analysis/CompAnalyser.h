//
// Created by wouter on 15-3-2024.
//

#ifndef ANALYSER_H
#define ANALYSER_H
#include <map>
#include <memory>

#include "../boid.h"
#include "../LanguageManager.h"
#include "../World.h"

using Matrix3D = std::vector<std::vector<std::vector<int>>>;

class CompAnalyser {
public:
    CompAnalyser(const World &world, int cell_size, std::vector<std::shared_ptr<KeyBoid>>& boids, LanguageManager& language_manager);
    ~CompAnalyser() = default;

    void LogAllMetrics(sf::Time delta_time);
    void LogBoidsPerLanguage();
    void LogLanguagesPerCell();
    void SaveBoidPerLanguageToCSV(const std::string& filename);
    void SaveLanguagesPerCellToCSV(const std::string& filename);
    void SetBPLTimeInterval(sf::Time interval);
    void SetLPCTimeInterval(sf::Time interval);

    std::vector<std::map<int, int>> boids_per_language;
    std::vector<Matrix3D> languages_per_cell;

private:
    std::vector<std::shared_ptr<KeyBoid>>& ref_boids;
    LanguageManager& ref_language_manager;
    int cell_size;
    int n_cells_in_width;
    int n_cells_in_height;

    sf::Time bpl_time_interval = sf::seconds(1.f);
    sf::Time lpc_time_interval = sf::seconds(1.f);
};



#endif //ANALYSER_H
