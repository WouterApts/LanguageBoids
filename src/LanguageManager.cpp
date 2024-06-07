//
// Created by wouter on 28-2-2024.
//

#include "LanguageManager.h"

#include <memory>
#include <utility>
#include <vector>

std::map<int, sf::Color> LanguageManager::InitLanguageColors() {
    return {
            {0, sf::Color::Red},
            {1, sf::Color::Green},
            {2, sf::Color(0, 100, 255)},
            {3, sf::Color::Yellow},
            {4, sf::Color::Cyan},
            {5, sf::Color::Magenta},
            {6, sf::Color(252, 152, 3)}, //Orange
            {7, sf::Color(249, 174, 252)}, //Light Pink
            {8, sf::Color(190, 255, 130)}, //Pastel Green
            {9, sf::Color(255, 255, 255)}, //White
    };
}

std::map<int, sf::Color> LanguageManager::language_colors = InitLanguageColors();

LanguageManager::LanguageManager(int number_of_languages) : n_languages(number_of_languages) {}

sf::Color LanguageManager::GetLanguageColor(int key) {
    if (auto color_iter = language_colors.find(key); color_iter != language_colors.end()) {
        return language_colors[key];
    } else {
        return sf::Color::Red;
    }
}

// Method to add a language to the language manager
void LanguageManager::AddLanguage(int key, std::shared_ptr<Language> language) {
    language_map[key] = std::move(language);
}

// Method to get a language from the language manager
std::shared_ptr<Language> LanguageManager::GetLanguage(int key) {
    auto it = language_map.find(key);
    if (it != language_map.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Language with key '" + std::to_string(key) + "' not found.");
    }
}

int LanguageManager::GetNumberOfLanguages() const {
    return n_languages;
}


