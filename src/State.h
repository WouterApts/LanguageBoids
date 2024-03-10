//
// Created by wouter on 19-2-2024.
//

#ifndef STATE_H
#define STATE_H

#include <SFML/System/Time.hpp>

class State {
public:
    State() = default;
    virtual ~State() = default;

    virtual void Init() = 0;
    virtual void ProcessInput() = 0;
    virtual void Update(sf::Time deltaTime) = 0;
    virtual void Pause() = 0;
    virtual void Draw() = 0;
    virtual void Start() = 0;

};
#endif //STATE_H
