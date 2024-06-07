//
// Created by wouter on 10-3-2024.
//

#include "Terrain.h"

#include <iostream>

#include "boid.h"
#include <SFML/Graphics/RenderWindow.hpp>

Terrain::Terrain(const std::vector<Eigen::Vector2f>& vertices, float friction_modifier, const std::pair<int, float> &language_status_modifier, float min_speed, float max_speed)
: vertices(vertices), friction_modifier(friction_modifier), language_status_modifier(language_status_modifier), min_speed(min_speed), max_speed(max_speed) {
    polygon.setPointCount(vertices.size());
    for (int i = 0; i < vertices.size(); ++i) {
        polygon.setPoint(i, sf::Vector2f(vertices[i].x(), vertices[i].y()));
    }
    auto terrain_gray = sf::Color(50,50,50, 150);
    polygon.setFillColor(terrain_gray);
}

bool Terrain::IsPointInside(const Eigen::Vector2f &point) const {
    int verticesCount = static_cast<int>(vertices.size());
    bool inside = false;

    for (int i = 0, j = verticesCount - 1; i < verticesCount; j = i++) {
        float intersection_x = (vertices[j].x() - vertices[i].x()) * (point.y() - vertices[i].y()) /
                               (vertices[j].y() - vertices[i].y()) + vertices[i].x();
        if (vertices[i].y() > point.y() != vertices[j].y() > point.y() && point.x() <= intersection_x) {
            inside = !inside;
        }
    }
    return inside;
}

void Terrain::ApplyMovementEffects(Boid *boid) const {
    Eigen::Vector2f acceleration = -boid->vel.normalized() * friction_modifier * boid->max_speed;
    boid->SetAcceleration(boid->acc + acceleration);
    boid->SetMinMaxSpeed(min_speed, max_speed);
}

void Terrain::ApplyLanguageStatusEffects(CompBoid *boid) const {
    boid->SetLanguageStatusMap(language_status_map);
}

void Terrain::InitLanguageStatusMap(const std::map<int, float> &map) {
    this->language_status_map = std::make_shared<std::map<int, float>>(map);
    (*this->language_status_map)[language_status_modifier.first] = language_status_modifier.second;
}

void Terrain::Draw(sf::RenderWindow* window) const {
    window->draw(polygon);
}

std::string Terrain::ToString() const {
    std::stringstream ss;
    ss << "Terrain: "
       << "min: " << min_speed << " "
       << "max: " << max_speed << " "
       << "friction: " << friction_modifier << " "
       << "vertices: " << static_cast<int>(vertices.size());
        for (auto v: vertices) {
            ss << " " << v.x() << " " << v.y();
        }
    ss << " languageMod: " << language_status_modifier.first << " " << language_status_modifier.second;
    return ss.str();
}

std::shared_ptr<Terrain> Terrain::FromString(const std::string& str) {
    std::istringstream iss(str);
    std::string type;
    std::string delimiter;
    int n_vertices;
    float min_speed;
    float max_speed;
    float friction_modifier;
    std::pair<int, float> language_status_modifier = {0, 1};
    std::vector<Eigen::Vector2f> vertices;
    if (!(iss >> type >> delimiter >> min_speed >> delimiter >> max_speed >> delimiter
    >> friction_modifier >> delimiter >> n_vertices)) {
        std::cerr << "Error: Invalid LineObstacle string format: " << str << std::endl;
        return nullptr;
    }

    // Parsing vertices
    for (int i = 0; i < n_vertices; ++i) {
        Eigen::Vector2f vertex;
        if (!(iss >> vertex.x() >> vertex.y())) {
            std::cerr << "Error: Invalid LineObstacle string format: " << str << std::endl;
            return nullptr;
        }
        vertices.push_back(vertex);
    }

    // Parsing language_status_modifier
    if (!(iss >> delimiter >> language_status_modifier.first >> language_status_modifier.second)) {
        std::cerr << "Error: Invalid language_status_modifiers in Terrain string format: " << str << std::endl;
        return nullptr;
    }
    return std::make_shared<Terrain>(vertices, friction_modifier, language_status_modifier, min_speed, max_speed);
}
