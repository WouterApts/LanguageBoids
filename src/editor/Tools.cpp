//
// Created by wouter on 16-3-2024.
//

#include "Tools.h"
#include "Editor.h"

Tool::Tool() = default;

LineObstacleTool::LineObstacleTool() = default;

void LineObstacleTool::OnLeftClick(sf::Vector2f tool_pos, World *world) {
    if (!building) {
        building = true;
        start_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
    } else {
        building = false;
        auto end_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
        auto line = std::make_shared<LineObstacle>(start_pos, end_pos, width, sf::Color::White);
        world->obstacles.push_back(line);
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


void CircleObstacleTool::OnLeftClick(sf::Vector2f tool_pos, World *world) {
    if (!building) {
        building = true;
        center_pos = Eigen::Vector2f(tool_pos.x, tool_pos.y);
    } else {
        building = false;
        auto world_pos =  Eigen::Vector2f(tool_pos.x, tool_pos.y);
        float radius = (center_pos - world_pos).norm();
        auto circle = std::make_shared<CircleObstacle>(center_pos, radius, sf::Color::White);
        world->obstacles.push_back(circle);
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


