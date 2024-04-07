//
// Created by wouter on 16-3-2024.
//

#ifndef EDITOR_H
#define EDITOR_H
#include <vector>

#include "Tools.h"
#include "State.h"
#include "Application.h"
#include "Camera.h"
#include "SimulationData.h"
#include "ToolSelector.h"
#include "ui/InterfaceManager.h"
#include "ui/components/TextField.h"

class Editor: public State {
public:

    std::shared_ptr<Context> context;
    Camera camera;

    SimulationData simulation_data;

    std::vector<int> grid_spacings = {0, 50, 100, 200, 500};
    int selected_grid_spacing = 2;
    int grid_spacing = grid_spacings[selected_grid_spacing];
    sf::Vector2f tool_pos{};

    std::shared_ptr<InterfaceManager> interface_manager;
    std::shared_ptr<TextField> type_text;
    std::shared_ptr<ToolSelector> tool_selector;


    Editor(const std::shared_ptr<Context> &context, const SimulationData &simulation_data, float camera_width, float camera_height);

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

    void SaveSimulationToFile() const;
};



#endif //EDITOR_H
