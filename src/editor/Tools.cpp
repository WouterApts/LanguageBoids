//
// Created by wouter on 16-3-2024.
//

#include "Tools.h"

#include <iostream>
#include <memory>

#include "BoidSpawners.h"
#include "Editor.h"
#include "LanguageManager.h"

Tool::Tool() = default;

LineObstacleTool::LineObstacleTool() = default;

void LineObstacleTool::Reset() {
    building = false;
}

void LineObstacleTool::OnLeftClick(sf::Vector2f tool_pos, World *world) {
    if (!building) {
        building = true;
        start_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
    } else {
        building = false;
        auto end_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
        if (!((end_pos - start_pos).norm() < 10.f)) {
            auto line = std::make_shared<LineObstacle>(start_pos, end_pos, width, sf::Color::White);
            world->obstacles.push_back(line);
        }
    }
}

void LineObstacleTool::OnRightClick(sf::Vector2f tool_pos, World *world) {
    // Cancel building
    building = false;
}

void LineObstacleTool::Draw(sf::Vector2f tool_pos, sf::RenderWindow* window) {
    // Draw line being build based on current mouse position
    if (building) {
        auto end_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
        auto line = LineObstacle(start_pos, end_pos, width, sf::Color(100,100,100));
        line.Draw(window);
    }
}


CircleObstacleTool::CircleObstacleTool() = default;

void CircleObstacleTool::Reset() {
    building = false;
}

void CircleObstacleTool::OnLeftClick(sf::Vector2f tool_pos, World *world) {
    if (!building) {
        building = true;
        center_pos = Eigen::Vector2f(tool_pos.x, tool_pos.y);
    } else {
        building = false;
        auto world_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
        float radius = (center_pos - world_pos).norm();
        if (radius >= 10.f) {
            auto circle = std::make_shared<CircleObstacle>(center_pos, radius, sf::Color::White);
            world->obstacles.push_back(circle);
        }
    }
}

void CircleObstacleTool::OnRightClick(sf::Vector2f tool_pos, World *world) {
    // Cancel building
    building = false;
}

void CircleObstacleTool::Draw(sf::Vector2f tool_pos, sf::RenderWindow* window) {
    // Draw circle being build based on current mouse position
    if (building) {
        auto world_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
        float radius = (center_pos - world_pos).norm();
        auto circle = CircleObstacle(center_pos, radius, sf::Color(100,100,100));
        circle.Draw(window);
    }
}


EraserTool::EraserTool() = default;

void EraserTool::OnLeftClick(sf::Vector2f tool_pos, World *world) {
    // Iterate through the obstacles in the world
    for (auto it = world->obstacles.begin(); it != world->obstacles.end(); ) {
        if ((*it)->CalcCollisionNormal(Eigen::Vector2f(tool_pos.x, tool_pos.y), this->brush_size)) {
            // If collision detected, erase the obstacle
            it = world->obstacles.erase(it); // Remove obstacle from the vector and advance iterator
        } else {
            // If no collision, move to the next obstacle
            ++it;
        }
    }
    for (auto it = world->terrains.begin(); it != world->terrains.end(); ) {
        if ((*it)->IsPointInside(Eigen::Vector2f(tool_pos.x, tool_pos.y))) {
            // If collision detected, erase the obstacle
            it = world->terrains.erase(it); // Remove obstacle from the vector and advance iterator
        } else {
            // If no collision, move to the next obstacle
            ++it;
        }
    }
    for (auto it = world->competition_boid_spawners.begin(); it != world->competition_boid_spawners.end(); ) {
        if ((*it)->IsInside(Eigen::Vector2f(tool_pos.x, tool_pos.y), this->brush_size)) {
            // If collision detected, erase the obstacle
            it = world->competition_boid_spawners.erase(it); // Remove obstacle from the vector and advance iterator
        } else {
            // If no collision, move to the next obstacle
            ++it;
        }
    }
}


TerrainTool::TerrainTool() = default;

void TerrainTool::Reset() {
    building = false;
    vertices.clear();
}

void TerrainTool::OnLeftClick(sf::Vector2f tool_pos, World *world) {
    if (!building) {
        vertices.emplace_back(tool_pos.x, tool_pos.y);
        building = true;
    } else {
        if ((Eigen::Vector2f(tool_pos.x, tool_pos.y) - vertices[0]).norm() < 10) {
            world->terrains.emplace_back(std::make_shared<Terrain>(vertices, friction_modifier, struggle_modifier, min_speed, max_speed));
            building = false;
            vertices.clear();
        } else {
            bool too_close_to_existing_vertex = false;
            for (int i = 1; i < vertices.size(); ++i) {
                if ((Eigen::Vector2f(tool_pos.x, tool_pos.y) - vertices[i]).norm() < 10) {
                    too_close_to_existing_vertex = true;
                    break;
                }
            }
            if (!too_close_to_existing_vertex) vertices.emplace_back(tool_pos.x, tool_pos.y);
        }
    }
}

void TerrainTool::OnRightClick(sf::Vector2f tool_pos, World *world) {
    building = false;
    vertices.clear();
}

void TerrainTool::Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) {
    if (building) {
        auto terrain_gray = sf::Color(50,50,50);
        float line_thickness = 3.f;

        auto start_dot = sf::CircleShape(10);
        start_dot.setOrigin(10,10);
        start_dot.setPosition(sf::Vector2f(vertices[0].x(), vertices[0].y()));
        start_dot.setFillColor(terrain_gray);
        window->draw(start_dot);

        if (vertices.size() < 2) {
            auto pos = Eigen::Vector2f(tool_pos.x, tool_pos.y);
            auto line = LineObstacle(vertices[0], pos, line_thickness, terrain_gray);
            line.Draw(window);
        } else {
            auto polygon = sf::ConvexShape();
            polygon.setPointCount(vertices.size() + 1);
            std::cout << vertices.size() + 1 << std::endl;
            for (int i = 0; i < vertices.size(); ++i) {
                polygon.setPoint(i, sf::Vector2f(vertices[i].x(), vertices[i].y()));
            }
            polygon.setPoint(vertices.size(), tool_pos);
            polygon.setFillColor(sf::Color(terrain_gray.r, terrain_gray.g, terrain_gray.b, 100.f));
            polygon.setOutlineColor(terrain_gray);
            polygon.setOutlineThickness(line_thickness);
            window->draw(polygon);
        }
    }
}

BoidTool::BoidTool() = default;

void BoidTool::Reset() {
    building = false;
}

void BoidTool::OnRightClick(sf::Vector2f tool_pos, World *world) {
    building = false;
}

BoidCircleTool::BoidCircleTool() = default;

void BoidCircleTool::OnLeftClick(sf::Vector2f tool_pos, World *world) {
    if (!building) {
        building = true;
        center_pos = Eigen::Vector2f(tool_pos.x, tool_pos.y);
    } else {
        building = false;
        auto world_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
        float radius = (center_pos - world_pos).norm();
        if (radius >= 1) {
            auto spawner = std::make_shared<CompBoidCircularSpawner>(boid_count, language_key, center_pos, radius);
            world->competition_boid_spawners.push_back(spawner);
        }
    }
}

void BoidCircleTool::Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) {
    auto color = LanguageManager::GetLanguageColor(language_key);
    color.a = 100;
    if (building) {
        auto world_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
        float radius = (center_pos - world_pos).norm();
        auto circle = CircleObstacle(center_pos, radius, color);
        circle.Draw(window);
    }
}





