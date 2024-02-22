//
// Created by wouter on 19-2-2024.
//


#include <StateManager.h>

StateManager::StateManager() : m_add(false), m_replace(false), m_remove(false) {

}

void StateManager::AddState(std::unique_ptr<State> state, bool replace) {
    m_add = true;
    m_newState = std::move(state);
    m_replace = replace;
};

void StateManager::PopState() {
    m_remove = true;
};

void StateManager::ProcessStateChange() {
    if (m_remove && !m_stateStack.empty()) {
        m_stateStack.pop();

        if (!m_stateStack.empty()) {
            m_stateStack.top()->Start();
        }

        m_remove = false;
    }

    if (m_add) {

        if (m_replace && !m_stateStack.empty()) {
            m_stateStack.pop();
            m_replace = false;
        }

        if (!m_stateStack.empty()) {
            m_stateStack.top()->Pause();
        }

        m_stateStack.push(std::move(m_newState));
        m_stateStack.top()->Init();
        m_stateStack.top()->Start();
        m_add = false;
    }
};

std::unique_ptr<State>& StateManager::GetCurrentState() {
    return m_stateStack.top();
};