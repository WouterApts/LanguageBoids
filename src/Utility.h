//
// Created by wouter on 25-2-2024.
//

#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <Eigen/Dense>

class InterfaceManager;

bool IsKeyPressedOnce(sf::Keyboard::Key keyCode);

float GetRandomFloatBetween(float min, float max);
int GetRandomIntBetween(int min, int max);

void PrintFPS(sf::Time delta_time);

void DisplayMessage(InterfaceManager interface_manager, sf::Vector2f pos, const std::string& message, sf::Time time, sf::Color text_color = sf::Color::White, int text_size = 20);

// Function to calculate the gradient color between green and red based on distance
sf::Color CalculateGradientColor(float distance);

struct vectorXiHash {
    std::size_t operator()(const Eigen::VectorXi& v) const {
        std::size_t seed = 0;
        for (int i = 0; i < v.size(); ++i) {
            seed ^= std::hash<int>()(v(i)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

// Comparison function for Eigen::VectorXi
struct vectorXiEqual {
    bool operator()(const Eigen::VectorXi& a, const Eigen::VectorXi& b) const {
        return a.isApprox(b);
    }
};

#endif //UTILITY_H
