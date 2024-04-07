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

template<typename ObjType>
class SpatialGrid {
public:
    int cell_size;
    int max_possible_key;
    Eigen::Vector2i grid_dimensions;
    Eigen::Vector2i world_dimensions;
    bool is_visible;

    std::map<int,int> num_offsets_within_distance;
    std::vector<int> global_offset;
    int max_d2;

    int CreateKeyFromIndex(int x, int y) const;
    Eigen::Vector2i GetIndex(Eigen::Vector2f position);

    std::map<int, std::unordered_set<std::shared_ptr<ObjType>>> grid;

    SpatialGrid(Eigen::Vector2i world_dim, int cell_size);
    ~SpatialGrid() = default;
    void Clear();

    void AddObj(const std::shared_ptr<ObjType>& obj);
    void RemoveObj(const std::shared_ptr<ObjType> &obj);
    void UpdateObj(const std::shared_ptr<ObjType> &obj);

    std::vector<ObjType*> ObjRadiusSearch(float query_radius, const std::shared_ptr<ObjType> &obj);
    std::vector<ObjType*> PosRadiusSearch(float query_radius, Eigen::Vector2f position);
    std::vector<ObjType*> LocalSearch(Eigen::Vector2f position);

    void DrawGrid(sf::RenderWindow* window);
};


#endif //SPATIALGRID_H
