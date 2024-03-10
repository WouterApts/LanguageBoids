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
            {2, sf::Color::Blue},
            {3, sf::Color::Yellow}
    };
}

std::map<int, sf::Color> LanguageManager::language_colors = LanguageManager::InitLanguageColors();

LanguageManager::LanguageManager(const std::vector<float>& language_statuses) {
    for (int i = 0; i < language_statuses.size(); ++i) {
        AddLanguage(i, std::make_shared<Language>(language_colors[i], language_statuses[i]));
    }
};


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

