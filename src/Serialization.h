//
// Created by wouter on 21-3-2024.
//

#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "World.h"
#include <fstream>

#include "SimulationData.h"

namespace serialization {
    bool SaveSimulationDataToFile(const SimulationData &data);

    std::optional<SimulationData> LoadSimulationDataFromFile(const std::string &filename);
    std::string GetFileNameThroughSaveDialog();
    std::string GetFileNameThroughLoadDialog();
};


#endif //SERIALIZATION_H
