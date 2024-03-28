//
// Created by wouter on 19-3-2024.
//

#include "InterfaceManager.h"

#include <iostream>

void InterfaceManager::AddComponent(const std::shared_ptr<InterfaceComponent>& component) {
    components.push_back(component);
}

void InterfaceManager::RemoveComponent(const std::shared_ptr<InterfaceComponent> &component) {
    for (auto it = components.begin(); it != components.end(); ) {
        if (it->get() == component.get()) {
            it = components.erase(it);
        } else {
            ++it;
        }
    }
}

void InterfaceManager::DrawComponents(sf::RenderWindow *window) {
    for (auto& component : components) {
        if (component->active) {
            component->Draw(window);
        }
    }
}

void InterfaceManager::OnLeftClick(sf::Vector2f mouse_pos) {
    for (auto& component : components) {
        if (component->active & component->IsPointInside(mouse_pos)) {
            std::cout << "clicked" << std::endl;
            component->OnLeftClick(mouse_pos);
            interface_clicked = true;
        }
    }
}

void InterfaceManager::OnRightClick(sf::Vector2f mouse_pos) {
    for (auto& component : components) {
        if (component->active) {
            component->OnRightClick(mouse_pos);
        }
    }
}

void InterfaceManager::OnMouseEnter(sf::Vector2f mouse_pos) {
    for (auto& component : components) {
        if (component->active) {
            component->OnMouseEnter(mouse_pos);
        }
    }
}

void InterfaceManager::OnMouseLeave(sf::Vector2f mouse_pos) {
    for (auto& component : components) {
        if (component->active) {
            component->OnMouseLeave(mouse_pos);
        }
    }
}

void InterfaceManager::OnKeyBoardEnter(sf::Uint32 unicode) {
    for (auto& component : components) {
        if (component->active) {
            component->OnKeyBoardEnter(unicode);
        }
    }
}
