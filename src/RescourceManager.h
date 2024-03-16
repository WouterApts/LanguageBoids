//
// Created by wouter on 16-3-2024.
//

#ifndef RESCOURCEMANAGER_H
#define RESCOURCEMANAGER_H

#include <memory>
#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>

class RescourceManager {
public:
    static void LoadTexture(const std::string& id, const std::string& filename);
    static sf::Texture* GetTexture(const std::string& id);

    // Public getter method for textures_
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>>& GetTextures();

private:
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
};

#endif //RESCOURCEMANAGER_H
