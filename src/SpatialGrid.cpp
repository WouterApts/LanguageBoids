//
// Created by wouter on 21-2-2024.
//

#include "SpatialGrid.h"

#include <cmath>
#include <iostream>
#include <utility>

SpatialGrid::SpatialGrid(Eigen::Vector2i world_dimensions, int cell_size)
    : cell_size(cell_size), world_dimensions(std::move(world_dimensions)) {

    grid_dimensions.x() = ceil(this->world_dimensions.x() / static_cast<double>(cell_size));
    grid_dimensions.y() = ceil(this->world_dimensions.y() / static_cast<double>(cell_size));

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
            int d_x = std::max((abs(colIndex - center.x())-1),0); // min.horizontal dist between this cell to center cell
            int d_y = std::max((abs(rowIndex - center.y())-1),0); // min.vertical dist between this cell to center cell
            int dist_squared = static_cast<int>(std::pow(d_x, 2) + std::pow(d_y, 2));
            offsets_by_distance_to_center[dist_squared].insert(key - center_key);

            max_dist_squared = std::max(dist_squared, max_dist_squared); // record max squaredDist
        }
    }

    // TODO: possible change this to a hashmap, because there will be large 'gaps' between different values.
    num_offsets_within_distance = std::vector<int>(max_dist_squared + 1);

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

int SpatialGrid::CreateKeyFromIndex(int x, int y) {
    return x + y * grid_dimensions.x();
}


Eigen::Vector2i SpatialGrid::GetIndex(Eigen::Vector2f position) {
    float cX = position.x() / world_dimensions.x();
    float cY = position.y() / world_dimensions.y();

    int xIndex = static_cast<int>(std::floor(cX * (grid_dimensions.x() - 1)));
    int yIndex = static_cast<int>(std::floor(cY * (grid_dimensions.y() - 1)));

    return {xIndex, yIndex};
}


void SpatialGrid::AddBoid(const std::shared_ptr<Boid> &boid) {
    // position of the base of boid arrow
    auto index = GetIndex(boid->pos);
    int key = CreateKeyFromIndex(index.x(), index.y());

    grid[key].insert(boid);
    boid->spatial_key = key;
}


void SpatialGrid::UpdateBoid(const std::shared_ptr<Boid> &boid) {
    auto index = GetIndex(boid->pos);
    int key = CreateKeyFromIndex(index.x(), index.y());

    if(key != boid->spatial_key) {
        RemoveBoid(boid);
        grid[key].insert(boid);
        boid->spatial_key = key;
    }
}


void SpatialGrid::RemoveBoid(const std::shared_ptr<Boid> &boid) {
    grid[boid->spatial_key].erase(boid);
}


std::vector<std::shared_ptr<Boid>> SpatialGrid::RadiusSearch(const std::shared_ptr<Boid> &boid, float query_radius) {

    std::vector<std::shared_ptr<Boid>> boids_in_radius;

    double d = query_radius / static_cast<double>(cell_size); // convert radius in pixel space to grid space
    int d2 = std::floor(d*d);

    int cell_key_of_boid = boid->spatial_key;

    for(int i = 0; i < num_offsets_within_distance[d2]; i++) {
        // Get candidate cell's key by adding the appropriate offset to the cell_key of the querying boid
        int key = cell_key_of_boid + global_offset[i];
        // Make sure the key is within the grid bounds
        key = std::min<int>(std::max(0, key), max_possible_key);

        // Check boids within candidate cells
        for(const auto& other_boid : grid[key]) {
            if(other_boid.get() == boid.get()) continue;

            Eigen::Vector2f difference = (boid->pos - other_boid->pos);
            float squared_distance = difference.squaredNorm();
            if (squared_distance <= query_radius * query_radius) boids_in_radius.push_back(other_boid);
        }
    }

    return boids_in_radius;
}

void SpatialGrid::Clear() {
    grid.clear();
}