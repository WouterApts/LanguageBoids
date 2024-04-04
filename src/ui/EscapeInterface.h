//
// Created by wouter on 25-3-2024.
//

#ifndef ESCAPEINTERFACE_H
#define ESCAPEINTERFACE_H
#include "Application.h"
#include "components/Panel.h"


class EscapeInterface : public Panel {
public:
    float button_size[2] = {100, 50};
    EscapeInterface(sf::Vector2f pos, std::shared_ptr<Context> &context);
};



#endif //ESCAPEINTERFACE_H
