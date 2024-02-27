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

template<typename BoidType>
class SpatialGrid {
public:
    int cell_size;
    int max_possible_key;
    Eigen::Vector2i grid_dimensions;
    Eigen::Vector2i world_dimensions;
    bool is_visible;

    std::vector<int> num_offsets_within_distance;
    std::vector<int> global_offset;

    int CreateKeyFromIndex(int x, int y) const;
    Eigen::Vector2i GetIndex(Eigen::Vector2f position);

    std::map<int, std::unordered_set<std::shared_ptr<BoidType>>> grid;

    SpatialGrid(Eigen::Vector2i world_dim, int cell_size);
    ~SpatialGrid() = default;
    void Clear();

    void AddBoid(const std::shared_ptr<BoidType>& boid);
    void RemoveBoid(const std::shared_ptr<BoidType> &boid);
    void UpdateBoid(const std::shared_ptr<BoidType> &boid);

    std::vector<BoidType*> RadiusSearch(float query_radius, const std::shared_ptr<BoidType> &boid = nullptr);
    std::vector<BoidType*> LocalSearch(Eigen::Vector2f position);

    void DrawGrid(sf::RenderWindow* window);
};


#endif //SPATIALGRID_H
