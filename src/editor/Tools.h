//
// Created by wouter on 16-3-2024.
//

#ifndef TOOL_H
#define TOOL_H
#include "SimulationData.h"
#include "World.h"

class Editor;

class Tool {
public:
    Tool();
    virtual ~Tool() = default;

    virtual void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) {}
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
    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;
};


class CircleObstacleTool : public Tool {
public:
    CircleObstacleTool();

    bool building = false;
    Eigen::Vector2f center_pos;

    void Reset() override;
    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;
};


class EraserTool : public Tool {
public:
    EraserTool();

    float brush_size = 10;

    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;
};


class TerrainTool : public Tool {
public:
    TerrainTool();

    bool building = false;
    std::vector<Eigen::Vector2f> vertices;
    float friction_modifier = 1;
    float struggle_modifier = 1;
    int min_speed = 75;
    int max_speed = 100;

    void Reset() override;
    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;

};


class BoidTool : public Tool {
public:
    BoidTool();

    bool building = false;
    int boid_count = 100;
    // Key Boids
    int language_key = 0;
    // Vector Boids
    int language_seed = 42;
    float feature_bias = 0.5;

    void Reset() override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;

};


class KeyBoidCircleTool : public BoidTool {
public:
    KeyBoidCircleTool();

    Eigen::Vector2f center_pos;

    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;

};

class KeyBoidRectangleTool : public BoidTool {
public:
    KeyBoidRectangleTool();

    Eigen::Vector2f start_pos;

    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;

};

class StudyBoidCircleTool : public KeyBoidCircleTool {
public:
    StudyBoidCircleTool();
    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;

};

class StudyBoidRectangleTool : public KeyBoidRectangleTool {
public:
    StudyBoidRectangleTool();
    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;

};

class VectorBoidCircleTool : public BoidTool {
public:
    VectorBoidCircleTool();

    Eigen::Vector2f center_pos;

    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;
};

class VectorBoidRectangularTool : public BoidTool {
public:
    VectorBoidRectangularTool();

    Eigen::Vector2f start_pos;

    void OnLeftClick(sf::Vector2f tool_pos, SimulationData & simulation_data) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;

};
#endif //TOOL_H
