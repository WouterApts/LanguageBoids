//
// Created by wouter on 25-3-2024.
//

#include "EscapeStateInterface.h"

#include "components/Button.h"
#include "ResourceManager.h"
#include "components/TextField.h"


EscapeStateInterface::EscapeStateInterface(sf::Vector2f pos, std::shared_ptr<Context>& context, const std::string& message, int text_size)
    : Panel(pos, sf::Vector2f(400, 200)) {

    std::vector<std::shared_ptr<InterfaceComponent> > btns;
    btns.emplace_back(std::make_shared<TextButton>([&context]() { context->state_manager->PopState();},
                                                "YES", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1]), 20.f));
    btns.emplace_back(std::make_shared<TextButton>([&]() { this->Deactivate(); },
                                                "NO", sf::Vector2f(0,0), sf::Vector2f(button_size[0], button_size[1]), 20.f));

    for (int i = 0; i < btns.size(); ++i) {
        int offset = static_cast<int>(button_size[0]) + 50;
        AddComponentWithRelativePos(btns[i], sf::Vector2f(75 + offset*i,100));
    }

    std::shared_ptr<TextField> text = std::make_shared<TextField>(sf::Vector2f(0,0), text_size, message, *ResourceManager::GetFont("arial"), sf::Color::White);
    float width = text->text.getGlobalBounds().width;
    auto text_fld_pos = sf::Vector2f((this->rect.getSize().x - width)/2.f,20);
    AddComponentWithRelativePos(text, text_fld_pos);
}
