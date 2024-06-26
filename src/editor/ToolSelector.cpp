//
// Created by wouter on 18-3-2024.
//

#include "ToolSelector.h"
#include "Editor.h"
#include "ui/ToolSelectorInterface.h"

ToolSelector::ToolSelector(SimulationData& simulation_data)
    : selected_tool(EraserT), simulation_data(simulation_data) {
    tools[EraserT] = std::make_shared<EraserTool>();
    tools[LineObstacleT] = std::make_shared<LineObstacleTool>();
    tools[CircleObstacleT] = std::make_shared<CircleObstacleTool>();
    tools[TerrainT] = std::make_shared<TerrainTool>();
    tools[KeyBoidCircleT] = std::make_shared<CompBoidCircleTool>();
    tools[KeyBoidRectangleT] = std::make_shared<CompBoidRectangleTool>();
    tools[VectorBoidCircleT] = std::make_shared<EvoBoidCircleTool>();
    tools[VectorBoidRectangleT] = std::make_shared<EvoBoidRectangleTool>();
    tools[StudyBoidCircleT] = std::make_shared<StudyBoidCircleTool>();
    tools[StudyBoidRectangleT] = std::make_shared<StudyBoidRectangleTool>();
}

void ToolSelector::SelectTool(Tools tool) {
    if (tool == BoidCircleT) {
        if (simulation_data.type == CompSimulation) {
            selected_tool = KeyBoidCircleT;
        }
        else if (simulation_data.type == EvoSimulation) {
            selected_tool = VectorBoidCircleT;
        }
        else if (simulation_data.type == CompStudy) {
            selected_tool = StudyBoidCircleT;
        }
    }
    else if (tool == BoidRectangleT) {
        if (simulation_data.type == CompSimulation) {
            selected_tool = KeyBoidRectangleT;
        }
        else if (simulation_data.type == EvoSimulation) {
            selected_tool = VectorBoidRectangleT;
        }
        else if (simulation_data.type == CompStudy) {
            selected_tool = StudyBoidRectangleT;
        }
    }
    else selected_tool = tool;

    tools[selected_tool]->Reset();
    if (tool_selector_interface) {
        tool_selector_interface->ActivateSelectedToolInterface();
    }
}

Tool* ToolSelector::GetSelectedTool() {
    return tools[selected_tool].get();
}


