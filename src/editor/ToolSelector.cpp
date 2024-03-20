//
// Created by wouter on 18-3-2024.
//

#include "ToolSelector.h"


ToolSelector::ToolSelector() : selected_tool(EraserT) {
    tools[EraserT] = std::make_shared<EraserTool>();
    tools[LineObstacleT] = std::make_shared<LineObstacleTool>();
    tools[CircleObstacleT] = std::make_shared<CircleObstacleTool>();
    tools[TerrainT] = std::make_shared<TerrainTool>();
}

void ToolSelector::SelectTool(Tools tool) {
    selected_tool = tool;
    tools[selected_tool]->Reset();
}

Tool* ToolSelector::GetSelectedTool() {
    return tools[selected_tool].get();
}


