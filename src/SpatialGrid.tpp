//
// Created by wouter on 21-2-2024.
//

#ifndef SPATIALGRID_TPP
#define SPATIALGRID_TPP

#include <cmath>
#include <iostream>
#include <utility>

#include "ResourceManager.h"
#include "SpatialGrid.h"

template <typename ObjType>
SpatialGrid<ObjType>::SpatialGrid(Eigen::Vector2i world_dimensions, int cell_size)
    : cell_size(cell_size), world_dimensions(std::move(world_dimensions)), is_visible(false){

    // To allow for maximum query distances, the grid length is doubled in both dimensions
    grid_dimensions.x() = ceil(this->world_dimensions.x() * 2 / static_cast<double>(cell_size));
    grid_dimensions.y() = ceil(this->world_dimensions.y() * 2 / static_cast<double>(cell_size));

    Eigen::Vector2i center = grid_dimensions / 2;
    int center_key = CreateKeyFromIndex(center.x(), center.y());

    std::map<int, std::unordered_set<int>> offsets_by_distance_to_center {};
    int max_dist_squared = 0;

    for (int rowIndex = 0; rowIndex < grid_dimensions.y(); ++rowIndex) {
        for (int colIndex = 0; colIndex < grid_dimensions.x(); ++colIndex) {
            if (rowIndex == center.y() && colIndex == center.x()) {
                offsets_by_distance_to_center[0].insert(0);
                continue;
            }
            int key = CreateKeyFromIndex(colIndex, rowIndex);
            int d_x = std::max((abs(colIndex - center.x())-1),0); // min. horizontal dist between this cell to center cell
            int d_y = std::max((abs(rowIndex - center.y())-1),0); // min. vertical dist between this cell to center cell
            int dist_squared = static_cast<int>(std::pow(d_x, 2) + std::pow(d_y, 2));
            offsets_by_distance_to_center[dist_squared].insert(key - center_key);

            max_dist_squared = std::max(dist_squared, max_dist_squared); // record max squaredDist
        }
    }

    max_d2 = max_dist_squared;

    int num_cells = grid_dimensions.x() * grid_dimensions.y();
    global_offset = std::vector<int>(num_cells);
    max_possible_key = num_cells - 1;

    int n = 0;
    // d2 stands for distance squared
    for(int d2 = 0; d2 <= max_dist_squared; ++d2) {
        if(offsets_by_distance_to_center[d2].empty()) {
            // if there are no cells whose min. squared dist. to center is d2:
            num_offsets_within_distance[d2] = num_offsets_within_distance[d2-1];
        } else {
            for(auto& offset : offsets_by_distance_to_center[d2]) {
                global_offset[n] = offset;
                n++;
            }
            num_offsets_within_distance[d2] = n;
        }
    }
}

template <typename ObjType>
int SpatialGrid<ObjType>::CreateKeyFromIndex(int x, int y) const {
    return x + y * grid_dimensions.x();
}

template <typename ObjType>
Eigen::Vector2i SpatialGrid<ObjType>::GetIndex(Eigen::Vector2f position) {

    float cX = (position.x()) / (world_dimensions.x() * 2);
    float cY = (position.y()) / (world_dimensions.y() * 2);

    int xIndex = static_cast<int>(std::floor(cX * grid_dimensions.x()));
    int yIndex = static_cast<int>(std::floor(cY * grid_dimensions.y()));

    return {xIndex, yIndex};
}

template <typename ObjType>
void SpatialGrid<ObjType>::DrawGrid(sf::RenderWindow* window) {
    if (is_visible) {
        for (int r = 0; r < grid_dimensions.y(); ++r) {
            for (int c = 0; c < grid_dimensions.x(); ++c) {
                int key = CreateKeyFromIndex(c, r);
                if (int num_boids = grid[key].size(); num_boids > 0) {
                    // Calculate grayscale color value based on number of obj
                    int grayscale_value = std::min(num_boids * 10, 255);

                    sf::RectangleShape rect(sf::Vector2f(cell_size, cell_size));
                    rect.setPosition(c * cell_size, r * cell_size);
                    rect.setFillColor(sf::Color::Transparent);

                    // Set outline color based on grayscale value
                    rect.setOutlineColor(sf::Color(grayscale_value, grayscale_value, grayscale_value));
                    rect.setOutlineThickness(3);
                    window->draw(rect);

                }
            }
        }
    }
}

template<typename ObjType>
void SpatialGrid<ObjType>::AddObj(const std::shared_ptr<ObjType>& obj) {
    auto index = GetIndex(obj->pos);
    int key = CreateKeyFromIndex(index.x(), index.y());

    grid[key].insert(obj);
    obj->spatial_key = key;
}

template<typename ObjType>
void SpatialGrid<ObjType>::UpdateObj(const std::shared_ptr<ObjType>& obj) {
    auto index = GetIndex(obj->pos);
    int key = CreateKeyFromIndex(index.x(), index.y());

    if(key != obj->spatial_key) {
        RemoveObj(obj);
        grid[key].insert(obj); // creates a copy of shared_ptr, assigning an additional owner (grid)
        obj->spatial_key = key;
    }
}

template<typename ObjType>
void SpatialGrid<ObjType>::RemoveObj(const std::shared_ptr<ObjType>& obj) {
    grid[obj->spatial_key].erase(obj);
}

template<typename ObjType>
std::vector<ObjType*> SpatialGrid<ObjType>::ObjRadiusSearch(float query_radius, const std::shared_ptr<ObjType>& obj) {

    std::vector<ObjType*> obj_in_radius;

    double d = query_radius / static_cast<double>(cell_size); // convert radius in pixel space to grid space
    int d2 = std::floor(d*d);
    if (d2 > max_d2) d2 = max_d2;

    float squared_query_radius = query_radius * query_radius;

    int obj_cell_key = obj->spatial_key;
    for(int i = 0; i < num_offsets_within_distance[d2]; i++) {
        // Get neighbouring cell's key by adding the appropriate offset to the cell_key of the querying object
        int key = obj_cell_key + global_offset[i];

        // Check if neighbour cell is within the grid bounds, if not disregard it and continue.
        if (key < 0 || key > max_possible_key) continue;

        // Check objects within the neighbour cell
        for(const auto& other_obj : grid[key]) {
            if(other_obj.get() == obj.get()) continue;

            Eigen::Vector2f difference = (obj->pos - other_obj->pos);
            float squared_distance = difference.squaredNorm();
            if (squared_distance <= squared_query_radius) {
                obj_in_radius.push_back(other_obj.get());
            }
            // else {
            //     // World wraps around both its vertical sides
            //     difference.x() = world_dimensions[0] - std::abs(obj->pos.x() - other_obj->pos.x());
            //     squared_distance = difference.squaredNorm();
            //     if (squared_distance <= squared_query_radius) {
            //         obj_in_radius.push_back(other_obj.get());
            //     }
            // }
        }
    }
    return obj_in_radius;
}

template<typename ObjType>
std::vector<ObjType*> SpatialGrid<ObjType>::PosRadiusSearch(float query_radius, Eigen::Vector2f position) {

    std::vector<ObjType*> obj_in_radius;

    double d = query_radius / static_cast<double>(cell_size); // convert radius in world space to grid space
    int d2 = std::floor(d*d);
    if (d2 > max_d2) d2 = max_d2;

    Eigen::Vector2i index = GetIndex(std::move(position));
    int key_of_position = CreateKeyFromIndex(index.x(), index.y());

    for(int i = 0; i < num_offsets_within_distance[d2]; i++) {

        // Get neighbouring cell's key by adding the appropriate offset to the cell_key of the querying object
        int key = key_of_position + global_offset[i];

        // Check if neighbour cell is within the grid bounds, if not disregard it and continue.
        if (key < 0 || key > max_possible_key) continue;

        // Check objects within the neighbour cell
        for(const auto& other_obj : grid[key]) {
            Eigen::Vector2f difference = (position - other_obj->pos);
            float squared_distance = difference.squaredNorm();
            if (squared_distance <= query_radius * query_radius) obj_in_radius.push_back(other_obj.get());
        }
    }
    return obj_in_radius;
}

template <typename ObjType>
std::vector<ObjType*> SpatialGrid<ObjType>::LocalSearch(Eigen::Vector2f position) {

    std::vector<ObjType*> objects_in_cell;

    Eigen::Vector2i index = GetIndex(std::move(position));
    int key_of_point = CreateKeyFromIndex(index.x(), index.y());
    for(int i = 0; i < num_offsets_within_distance[0]; i++) {
        int key = key_of_point + global_offset[i];
        key = std::min<int>(std::max(0, key), max_possible_key);

        // Check boids within candidate cells
        for(const auto& obj : grid[key]) {
            objects_in_cell.push_back(obj.get());
        }
    }

    return objects_in_cell;
}

template <typename ObjType>
void SpatialGrid<ObjType>::Clear() {
    grid.clear();
}

#endif //SPATIALGRID_TPP