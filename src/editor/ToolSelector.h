//
// Created by wouter on 18-3-2024.
//

#ifndef TOOLSELECTOR_H
#define TOOLSELECTOR_H
#include <map>
#include <string>
#include "Tools.h"

enum Tools {
    EraserT,
    LineObstacleT,
    CircleObstacleT,
    TerrainT,
    BoidCircleT,
    BoidRectangleT
};

class ToolSelector {
public:
    ToolSelector();
    std::map<Tools, std::shared_ptr<Tool>> tools;
    Tools selected_tool;

    void SelectTool(Tools tool);

    Tool *GetSelectedTool();
};



#endif //TOOLSELECTOR_H
