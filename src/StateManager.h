//
// Created by wouter on 19-2-2024.
//

#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include <memory>
#include <stack>

#include "State.h"

class StateManager {
private:
    std::stack<std::unique_ptr<State>> m_stateStack;
    std::unique_ptr<State> m_newState;

    bool m_add{};
    bool m_remove{};
    bool m_replace{};

public:

    StateManager();
    ~StateManager() = default;

    void AddState(std::unique_ptr<State> state, bool replace = false);
    void PopState();
    void ProcessStateChange();
    std::unique_ptr<State>& GetCurrentState();
};

#endif //STATEMANAGER_H
