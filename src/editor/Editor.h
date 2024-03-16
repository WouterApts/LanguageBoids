//
// Created by wouter on 16-3-2024.
//

#ifndef EDITOR_H
#define EDITOR_H
#include <vector>

#include "EditorTool.h"
#include "Obstacles.h"
#include "SpawnZone.h"
#include "State.h"

class Editor: State {
public:

    EditorTool selected_tool;
    std::vector<Obstacle> obstacles;
    std::vector<SpawnZone> spawn_zones;
    Editor();

    void Init() override;
    void ProcessInput() override;
    void Update(sf::Time deltaTime) override;
    void Pause() override;
    void Draw() override;
    void Start() override;

};



#endif //EDITOR_H
