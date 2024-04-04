//
// Created by wouter on 18-3-2024.
//

#include "ToolSelector.h"

#include "Editor.h"


ToolSelector::ToolSelector(SimulationData& simulation_data)
    : selected_tool(EraserT), simulation_data(simulation_data) {
    tools[EraserT] = std::make_shared<EraserTool>();
    tools[LineObstacleT] = std::make_shared<LineObstacleTool>();
    tools[CircleObstacleT] = std::make_shared<CircleObstacleTool>();
    tools[TerrainT] = std::make_shared<TerrainTool>();
    tools[KeyBoidCircleT] = std::make_shared<KeyBoidCircleTool>();
    tools[KeyBoidRectangleT] = std::make_shared<KeyBoidRectangleTool>();
    tools[StudyBoidCircleT] = std::make_shared<StudyBoidCircleTool>();
    tools[StudyBoidRectangleT] = std::make_shared<StudyBoidRectangleTool>();
}

void ToolSelector::SelectTool(Tools tool) {
    selected_tool = tool;
    tools[selected_tool]->Reset();
}

Tool* ToolSelector::GetSelectedTool() {
    return tools[selected_tool].get();
}


