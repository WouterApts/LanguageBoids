//
// Created by wouter on 16-3-2024.
//

#ifndef EDITOR_H
#define EDITOR_H
#include <vector>

#include "Tools.h"
#include "Obstacles.h"
#include "SpawnZone.h"
#include "State.h"
#include "ui/Interface.h"
#include "Application.h"
#include "Camera.h"

class Editor: public State {
public:

    std::shared_ptr<Context> context;
    Camera camera;
    World world;

    int grid_spacing = 100;
    sf::Vector2f tool_pos{};

    std::unique_ptr<Tool> p_selected_tool;
    std::vector<std::shared_ptr<Interface>> interfaces;
    std::vector<std::shared_ptr<SpawnZone>> spawn_zones;


    Editor(const std::shared_ptr<Context> &context, World &world, float camera_width, float camera_height);

    void Initialize_Tools_interface();
    void SelectTool(Tools tool);

    sf::Vector2f MapPixelToClosestGridCoords(sf::Vector2i pixel_coord) const;
    void DrawCursorHighlightOnGrid() const;
    void DrawGrid(sf::RenderWindow *window) const;

    void Init() override;
    void ProcessInput() override;
    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;
};



#endif //EDITOR_H
