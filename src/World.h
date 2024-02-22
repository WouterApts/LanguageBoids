//
// Created by wouter on 22-2-2024.
//

#ifndef WORLD_H
#define WORLD_H

#include <Eigen/Dense>

struct World {
    float width;
    float height;
    Eigen::Vector2f size() const {
        return {width, height};
    }
};

#endif //WORLD_H
