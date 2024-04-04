//
// Created by wouter on 18-3-2024.
//

#ifndef TOOLSELECTOR_H
#define TOOLSELECTOR_H
#include <map>
#include "Tools.h"

enum Tools {
    EraserT,
    LineObstacleT,
    CircleObstacleT,
    TerrainT,

    BoidCircleT,
    BoidRectangleT,

    KeyBoidCircleT,
    KeyBoidRectangleT,
    VectorBoidCircleT,
    VectorBoidRectangleT,
    StudyBoidCircleT,
    StudyBoidRectangleT,
};

class ToolSelector {
public:

    std::map<Tools, std::shared_ptr<Tool>> tools;
    Tools selected_tool;
    SimulationData& simulation_data;

    explicit ToolSelector(SimulationData& simulation_data);
    void SelectTool(Tools tool);

    Tool *GetSelectedTool();
};



#endif //TOOLSELECTOR_H
