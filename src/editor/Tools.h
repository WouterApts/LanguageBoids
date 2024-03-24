//
// Created by wouter on 16-3-2024.
//

#ifndef TOOL_H
#define TOOL_H
#include "Config.h"
#include "World.h"

class Editor;

class Tool {
public:
    Tool();
    virtual ~Tool() = default;

    virtual void OnLeftClick(sf::Vector2f tool_pos, World *world) {}

    virtual void Reset() {};
    virtual void OnRightClick(sf::Vector2f tool_pos, World *world) {};
    virtual void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) {};
};


class LineObstacleTool : public Tool {
public:
    LineObstacleTool();

    bool building = false;
    float width = 10.f;
    Eigen::Vector2f start_pos;

    void Reset() override;
    void OnLeftClick(sf::Vector2f tool_pos, World *world) override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;
};


class CircleObstacleTool : public Tool {
public:
    CircleObstacleTool();

    bool building = false;
    Eigen::Vector2f center_pos;

    void Reset() override;
    void OnLeftClick(sf::Vector2f tool_pos, World *world) override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;
};


class EraserTool : public Tool {
public:
    EraserTool();

    float brush_size = 10;

    void OnLeftClick(sf::Vector2f tool_pos, World *world) override;
};


class TerrainTool : public Tool {
public:
    TerrainTool();

    bool building = false;
    std::vector<Eigen::Vector2f> vertices;
    float friction_modifier = 1;
    float struggle_modifier = 1;
    int min_speed = MIN_SPEED;
    int max_speed = MAX_SPEED;

    void Reset() override;
    void OnLeftClick(sf::Vector2f tool_pos, World *world) override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;

};


class BoidTool : public Tool {
public:
    BoidTool();

    bool building = false;
    int boid_count = 100;
    int language_key = 0;

    void Reset() override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;

};


class BoidCircleTool : public BoidTool {
public:
    BoidCircleTool();

    Eigen::Vector2f center_pos;

    void OnLeftClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;

};

class BoidRectangleTool : public BoidTool {
public:
    BoidRectangleTool();

    Eigen::Vector2f start_pos;

    void OnLeftClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;

};
#endif //TOOL_H
