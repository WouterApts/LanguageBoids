//
// Created by wouter on 28-2-2024.
//

#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <map>
#include <unordered_map>
#include <memory>
#include <bits/stl_vector.h>
#include <SFML/Graphics/Color.hpp>


struct Language {
    sf::Color color;
    float popularity_status{};
};

class LanguageManager {
private:
    // Static map shared among all instances of LanguageManager
    static std::map<int, sf::Color> InitLanguageColors();
    static std::map<int, sf::Color> language_colors;
    std::unordered_map<int, std::shared_ptr<Language>> language_map;
    int n_languages;

public:

    explicit LanguageManager(const std::vector<float>& language_statuses);

    // Method to add a language to the language manager
    void AddLanguage(int key, std::shared_ptr<Language> language);
    std::shared_ptr<Language> GetLanguage(int key);

    int GetNumberOfLanguages() const;

};
#endif //LANGUAGEMANAGER_H
