#include "RescourceManager.h"

// Define static member variable
std::unordered_map<std::string, std::unique_ptr<sf::Texture>> RescourceManager::textures;

// Define static method LoadTexture
void RescourceManager::LoadTexture(const std::string& id, const std::string& filename) {
    std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();
    if (texture->loadFromFile(filename)) {
        textures[id] = std::move(texture); // No need for RescourceManager::GetTextures()
    }
}

// Define static method GetTexture
sf::Texture* RescourceManager::GetTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second.get();
    }
    return nullptr;
}

// Define static method GetTextures
std::unordered_map<std::string, std::unique_ptr<sf::Texture>>& RescourceManager::GetTextures() {
    return textures;
}

