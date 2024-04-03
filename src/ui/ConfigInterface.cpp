//
// Created by wouter on 3-4-2024.
//

#include "ConfigInterface.h"

#include "Button.h"
#include "ResourceManager.h"
#include "TextField.h"


ConfigInterface::ConfigInterface(std::shared_ptr<InterfaceManager>& interface_manager, sf::Vector2f pos, SimulationData& simulation_data)
    : Panel(pos, {800, 650}, sf::Color(125,125,125)), simulation_data(simulation_data) {

    auto default_config = SimulationConfig();
    float fld_w = 100;
    float fld_h = 30;

    auto btn_size = sf::Vector2f(200, 100);
    float btn_txt_size = 20;

    auto active_config_position = sf::Vector2f(0, 150);
    auto active_config_size = sf::Vector2f(this->rect.getSize().x, 100);

    // Simulation Type Buttons:
    auto key_btn = std::make_shared<TextButton>([&](){ this->SwitchToKeySimulator(); }, "LANGUAGE KEY\n SIMULATOR", sf::Vector2f(0,0), btn_size, btn_txt_size);
    auto vector_btn = std::make_shared<TextButton>([&](){ this->SwitchToVectorSimulator(); }, "LANGUAGE VECTOR\n SIMULATOR", sf::Vector2f(0,0), btn_size, btn_txt_size);
    auto study_btn = std::make_shared<TextButton>([&](){ this->SwitchToDominanceStudy(); }, "DOMINANCE\n STUDY", sf::Vector2f(0,0), btn_size, btn_txt_size);
    AddComponentWithRelativePos(key_btn, {10,10});
    AddComponentWithRelativePos(vector_btn, {10 + btn_size.x + 50,10});
    AddComponentWithRelativePos(study_btn, {10 + 2*(btn_size.x + 50),10});

    // Default Config:
    CreateDefaultConfigFields(simulation_data, default_config, fld_w, fld_h, *interface_manager);

    // Language Key Configuration:
    key_config_interface = std::make_shared<Panel>(sf::Vector2f(0,0), active_config_size);
    auto a_coefficient_fld = std::make_shared<FloatInputField>(*interface_manager, [&simulation_data](float value){ simulation_data.config->a_COEFFICIENT = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.a_COEFFICIENT);
    std::shared_ptr<InterfaceComponent> a_coefficient_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "A Coefficient", *ResourceManager::GetFont("arial"), sf::Color::Black);
    auto influence_fld = std::make_shared<FloatInputField>(*interface_manager, [&simulation_data](float value){ simulation_data.config->INFLUENCE_RATE = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.INFLUENCE_RATE);
    std::shared_ptr<InterfaceComponent> influence_text = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Influence Rate", *ResourceManager::GetFont("arial"), sf::Color::Black);

    AddComponentWithRelativePos(key_config_interface, active_config_position);
    key_config_interface->AddComponentWithRelativePos(a_coefficient_fld, {10, 10});
    key_config_interface->AddComponentWithRelativePos(a_coefficient_text, {10 + fld_w + 10, 10});
    key_config_interface->AddComponentWithRelativePos(influence_fld, {10, 10 + fld_h + 10});
    key_config_interface->AddComponentWithRelativePos(influence_text, {10 + fld_w + 10, 10 + fld_h + 10});
    active_config = key_config_interface;

    this->Deactivate();
}

void ConfigInterface::Draw(sf::RenderWindow* window) {
    Panel::Draw(window);
    if (active_config) {
        active_config->Draw(window);
    }
}

void ConfigInterface::CreateDefaultConfigFields(SimulationData &simulation_data, SimulationConfig default_config, float fld_w, float fld_h, InterfaceManager& interface_manager) {
    std::vector<std::shared_ptr<InterfaceComponent>> flds;
    std::vector<std::shared_ptr<InterfaceComponent>> txts;
    auto coherence_factor_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->COHERENCE_FACTOR = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.COHERENCE_FACTOR);
    auto coherence_factor_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Coherence Factor", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto alignment_factor_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->ALIGNMENT_FACTOR = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.ALIGNMENT_FACTOR);
    auto alignment_factor_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Alignment Factor", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto separation_factor_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->SEPARATION_FACTOR = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.SEPARATION_FACTOR);
    auto separation_factor_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Separation Factor", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto avoidance_factor_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->AVOIDANCE_FACTOR = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.AVOIDANCE_FACTOR);
    auto avoidance_factor_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Avoidance Factor", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto max_speed_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->MAX_SPEED = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.MAX_SPEED);
    auto max_speed_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Boid MAX Speed", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto min_speed_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->MIN_SPEED = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.MIN_SPEED);
    auto min_speed_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Boid MIN Speed", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto perception_radius_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->PERCEPTION_RADIUS = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.PERCEPTION_RADIUS);
    auto perception_radius_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Perception Radius", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto interaction_radius_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->INTERACTION_RADIUS = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.INTERACTION_RADIUS);
    auto interaction_radius_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Interaction Radius", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto separation_radius_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->SEPARATION_RADIUS = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.SEPARATION_RADIUS);
    auto separation_radius_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Separation Radius", *ResourceManager::GetFont("arial"), sf::Color::Black);

    // Add fields and texts to their respective vectors
    flds.push_back(coherence_factor_fld);
    flds.push_back(alignment_factor_fld);
    flds.push_back(separation_factor_fld);
    flds.push_back(avoidance_factor_fld);
    flds.push_back(max_speed_fld);
    flds.push_back(min_speed_fld);
    flds.push_back(perception_radius_fld);
    flds.push_back(interaction_radius_fld);
    flds.push_back(separation_radius_fld);

    txts.push_back(coherence_factor_txt);
    txts.push_back(alignment_factor_txt);
    txts.push_back(separation_factor_txt);
    txts.push_back(avoidance_factor_txt);
    txts.push_back(max_speed_txt);
    txts.push_back(min_speed_txt);
    txts.push_back(perception_radius_txt);
    txts.push_back(interaction_radius_txt);
    txts.push_back(separation_radius_txt);

    // Default: Collision Physics
    auto boid_collision_radius_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->BOID_COLLISION_RADIUS = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.BOID_COLLISION_RADIUS);
    auto boid_collision_radius_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Boid Collision Radius", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto restitution_coefficient_fld = std::make_shared<FloatInputField>(interface_manager, [&simulation_data](float value){ simulation_data.config->RESTITUTION_COEFFICIENT = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.RESTITUTION_COEFFICIENT);
    auto restitution_coefficient_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Collision Restitution", *ResourceManager::GetFont("arial"), sf::Color::Black);

    // Add fields and texts to their respective vectors
    flds.push_back(boid_collision_radius_fld);
    txts.push_back(boid_collision_radius_txt);

    flds.push_back(restitution_coefficient_fld);
    txts.push_back(restitution_coefficient_txt);

    // Default: Analysis
    auto language_log_interval_fld = std::make_shared<IntInputField>(interface_manager, [&simulation_data](int value){ simulation_data.config->LANGUAGE_LOG_INTERVAL = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.LANGUAGE_LOG_INTERVAL);
    auto language_log_interval_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Language LOG Interval", *ResourceManager::GetFont("arial"), sf::Color::Black);

    auto position_log_interval_fld = std::make_shared<IntInputField>(interface_manager, [&simulation_data](int value){ simulation_data.config->POSITION_LOG_INTERVAL = value;}, sf::Vector2f(0,0), sf::Vector2f(fld_w, fld_h), 6, default_config.POSITION_LOG_INTERVAL);
    auto position_log_interval_txt = std::make_shared<TextField>(sf::Vector2f(0,0), 20, "Position LOG Interval", *ResourceManager::GetFont("arial"), sf::Color::Black);

    // Add fields and texts to their respective vectors
    flds.push_back(language_log_interval_fld);
    flds.push_back(position_log_interval_fld);

    txts.push_back(language_log_interval_txt);
    txts.push_back(position_log_interval_txt);

    for (int i = 0; i < flds.size(); ++i) {
        float x = 10 + (fld_w + 250) * std::floor(i/7);
        float y = 300 + (fld_h + 10) * static_cast<float>(i%7);
        AddComponentWithRelativePos(flds[i], {x, y});
        AddComponentWithRelativePos(txts[i], {x + fld_w + 10, y});
    }
}

void ConfigInterface::SwitchToKeySimulator() {
    if (active_config) {
        active_config->Deactivate();
    }
    active_config = key_config_interface;
    active_config->Activate();
}

void ConfigInterface::SwitchToVectorSimulator() {
    if (active_config) {
        active_config->Deactivate();
    }
    active_config = vector_config_interface;
    active_config->Activate();
}

void ConfigInterface::SwitchToDominanceStudy() {
    if (active_config) {
        active_config->Deactivate();
    }
    active_config = study_config_interface;
    active_config->Activate();
}
