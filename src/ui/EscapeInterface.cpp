//
// Created by wouter on 25-3-2024.
//

#include "EscapeInterface.h"

#include "Button.h"
#include "ResourceManager.h"
#include "TextField.h"


EscapeInterface::EscapeInterface(sf::Vector2f pos, std::shared_ptr<Context>& context)
    : Panel(pos, sf::Vector2f(400, 200), sf::Color(100,100,100)) {

    std::vector<std::shared_ptr<InterfaceComponent> > btns;
    btns.emplace_back(std::make_shared<TextButton>([&context]() { context->state_manager->PopState();},
                                                "YES", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1]), 20.f));
    btns.emplace_back(std::make_shared<TextButton>([&]() { this->active = false; },
                                                "NO", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1]), 20.f));

    for (int i = 0; i < btns.size(); ++i) {
        int offset = static_cast<int>(button_size[0]) + 50;
        AddComponentWithRelativePos(btns[i], sf::Vector2f(75 + offset*i,100));
    }

    std::shared_ptr<InterfaceComponent> text = std::make_shared<TextField>(sf::Vector2f(0,0), 30, "Exit Editor?", *ResourceManager::GetFont("arial"), sf::Color::White);
    AddComponentWithRelativePos(text, sf::Vector2f(130,20));
}
