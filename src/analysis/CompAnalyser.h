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

class CompAnalyser {
public:
    std::vector<std::map<int, int>> boids_per_language;
    std::vector<std::map<int, std::vector<Eigen::Vector2i>>> positions_per_language;

    CompAnalyser(std::vector<std::shared_ptr<CompBoid>>& boids);
    ~CompAnalyser() = default;

    void LogAllMetrics(sf::Time delta_time);
    void LogBoidsPerLanguage();
    void LogBoidPositions();

    void SaveBoidPerLanguageToCSV(const std::string& filename);
    void SavePositionPerLanguageCSV(const std::string &filename);

    void SetBPLTimeInterval(sf::Time interval);
    void SetPPLTimeInterval(sf::Time interval);

private:
    std::vector<std::shared_ptr<CompBoid>>& ref_boids;

    sf::Time bpl_time_interval = sf::seconds(1.f);
    sf::Time ppl_time_interval = sf::seconds(1.f);
};

#endif //ANALYSER_H
