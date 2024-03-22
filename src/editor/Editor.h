//
// Created by wouter on 16-3-2024.
//

#ifndef EDITOR_H
#define EDITOR_H
#include <vector>

#include "Tools.h"
#include "State.h"
#include "ui/Interface.h"
#include "Application.h"
#include "Camera.h"
#include "ToolSelector.h"

class Editor: public State {
public:

    std::shared_ptr<Context> context;
    Camera camera;
    World world;

    std::vector<int> grid_spacings = {0, 50, 100, 200, 500};
    int selected_grid_spacing = 0;
    int grid_spacing = grid_spacings[selected_grid_spacing];
    sf::Vector2f tool_pos{};

    std::vector<std::shared_ptr<Interface>> interfaces;
    std::shared_ptr<ToolSelector> tool_selector;


    Editor(const std::shared_ptr<Context> &context, World &world, float camera_width, float camera_height);

    sf::Vector2f MapPixelToClosestGridCoords(sf::Vector2i pixel_coord) const;
    void DrawCursorHighlightOnGrid() const;
    void DrawGrid(sf::RenderWindow *window) const;
    void NextGridSize();

    void Init() override;
    void ProcessInput() override;
    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;
};



#endif //EDITOR_H
