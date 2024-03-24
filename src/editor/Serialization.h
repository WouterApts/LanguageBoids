//
// Created by wouter on 21-3-2024.
//

#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "World.h"
#include <fstream>

namespace serialization {
    void SaveWorldToFile(const World &world);
    std::optional<World> LoadWorldFromFile(const std::string& filename);
    std::string GetFileNameThroughSaveDialog();
    std::string GetFileNameThroughLoadDialog();
};


#endif //SERIALIZATION_H
