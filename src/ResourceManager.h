//
// Created by wouter on 16-3-2024.
//

#ifndef RESCOURCEMANAGER_H
#define RESCOURCEMANAGER_H

#include <memory>
#include <unordered_map>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

class ResourceManager {
public:
    static void LoadTexture(const std::string& id, const std::string& filename);
    static void LoadFont(const std::string& id, const std::string& filename);
    static sf::Texture* GetTexture(const std::string& id);
    static sf::Font *GetFont(const std::string &id);

    // Public getter method for textures_
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>>& GetTextures();
    static std::unordered_map<std::string, std::unique_ptr<sf::Font>>& GetFonts();

private:
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    static std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
};

#endif //RESCOURCEMANAGER_H
