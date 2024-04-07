//
// Created by wouter on 6-4-2024.
//

#ifndef STUDYINTERFACE_H
#define STUDYINTERFACE_H
#include "components/Panel.h"
#include "components/TextField.h"


class StudyInterface : public Panel {
public:

    std::shared_ptr<TextField> run_fld;
    std::shared_ptr<TextField> simulation_time_fld;
    std::shared_ptr<TextField> distribution_fld;

    StudyInterface(sf::Vector2f pos);
};



#endif //STUDYINTERFACE_H
