//
// Created by wouter on 21-2-2024.
//

#ifndef SPATIALGRID_H
#define SPATIALGRID_H

#include <map>
#include <memory>
#include <unordered_set>

#include "Boid.h"
#include "Eigen/Dense"


class SpatialGrid {
public:
    int cell_size;
    int max_possible_key;
    Eigen::Vector2i grid_dimensions;
    Eigen::Vector2i world_dimensions;

    std::vector<int> num_offsets_within_distance;
    std::vector<int> global_offset;

    int CreateKeyFromIndex(int x, int y);
    Eigen::Vector2i GetIndex(Eigen::Vector2f position);
    std::map<int, std::unordered_set<std::shared_ptr<Boid>>> grid;

    SpatialGrid(Eigen::Vector2i world_dim, int cell_size);
    ~SpatialGrid() = default;

    void AddBoid(const std::shared_ptr<Boid>& boid);
    void RemoveBoid(const std::shared_ptr<Boid> &boid);
    void UpdateBoid(const std::shared_ptr<Boid> &boid);
    void Clear();
    std::vector<std::shared_ptr<Boid>> RadiusSearch(const std::shared_ptr<Boid> &boid, float query_radius);
};



#endif //SPATIALGRID_H
