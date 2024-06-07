//
// Created by wouter on 03/06/2024.
//

#ifndef EVOANALYSER_H
#define EVOANALYSER_H
#include <map>
#include <memory>
#include <vector>
#include <Eigen/Dense>
#include "SFML/System/Time.hpp"

class EvoBoid;

class EvoAnalyser {
public:
    EvoAnalyser(std::vector<std::shared_ptr<EvoBoid>>& boids);
    ~EvoAnalyser() = default;

    void SaveMetricsToCSV(const std::string &filename, sf::Time delta_time);
    void SetLogTimeInterval(sf::Time interval);

private:
    std::vector<std::shared_ptr<EvoBoid>>& ref_boids;
    sf::Time log_time_interval = sf::seconds(1.f);
};



#endif //EVOANALYSER_H
