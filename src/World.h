//
// Created by wouter on 22-2-2024.
//

#ifndef WORLD_H
#define WORLD_H

#include <Eigen/Dense>
#include <memory>
#include "Obstacles.h"
#include "Terrain.h"


class CompBoidSpawner;

struct World {
    float width;
    float height;
    std::vector<std::shared_ptr<Obstacle>> obstacles;
    std::vector<std::shared_ptr<Terrain>> terrains;

    [[nodiscard]] Eigen::Vector2f size() const {
        return {width, height};
    }
};

#endif //WORLD_H
