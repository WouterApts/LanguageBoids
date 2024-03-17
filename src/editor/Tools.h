//
// Created by wouter on 16-3-2024.
//

#ifndef TOOL_H
#define TOOL_H
#include "World.h"

class Editor;

enum class Tools {
    LineObstacleTool,
    CircleObstacleTool,
};

class Tool {
public:
    Tool();
    virtual ~Tool() = default;

    virtual void OnLeftClick(sf::Vector2f tool_pos, World *world) {};
    virtual void OnRightClick(sf::Vector2f tool_pos, World *world) {};
    virtual void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) {};
};

class LineObstacleTool : public Tool {
public:
    LineObstacleTool();

    bool building = false;
    float width = 3.f;
    Eigen::Vector2f start_pos;

    void OnLeftClick(sf::Vector2f tool_pos, World *world) override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;
};

class CircleObstacleTool : public Tool {
public:
    CircleObstacleTool();

    bool building = false;
    float width = 3.f;
    Eigen::Vector2f center_pos;

    void OnLeftClick(sf::Vector2f tool_pos, World *world) override;
    void OnRightClick(sf::Vector2f tool_pos, World *world) override;
    void Draw(sf::Vector2f tool_pos, sf::RenderWindow *window) override;
};

#endif //TOOL_H
