//
// Created by wouter on 22-2-2024.
//

#ifndef WORLD_H
#define WORLD_H

#include <Eigen/Dense>

#include "Terrain.h"

struct World {
    float width;
    float height;
    std::vector<Terrain*> terrains;
    Eigen::Vector2f size() const {
        return {width, height};
    }
};

#endif //WORLD_H
