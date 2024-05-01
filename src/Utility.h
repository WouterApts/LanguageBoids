//
// Created by wouter on 25-2-2024.
//

#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

class InterfaceManager;

bool IsKeyPressedOnce(sf::Keyboard::Key keyCode);

float GetRandomFloatBetween(float min, float max);
int GetRandomIntBetween(int min, int max);

void PrintFPS(sf::Time delta_time);

void DisplayMessage(InterfaceManager interface_manager, sf::Vector2f pos, const std::string& message, sf::Time time, sf::Color text_color = sf::Color::White, int text_size = 20);

// Function to calculate the gradient color between green and red based on distance
sf::Color CalculateGradientColor(float distance);

// Hash function for Eigen matrix and vector.
// The code is from `hash_combine` function of the Boost library. See
// http://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine .
template<typename T>
struct matrix_hash {
    std::size_t operator()(T const& matrix) const {
        size_t seed = 0;
        for (size_t i = 0; i < matrix.size(); ++i) {
            auto elem = *(matrix.data() + i);
            seed ^= std::hash<typename T::Scalar>()(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    bool operator()(const T& lhs, const T& rhs) const {
        return lhs.isApprox(rhs); // Or any other suitable comparison method
    }
};

#endif //UTILITY_H
