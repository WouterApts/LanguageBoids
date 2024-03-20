#include "ResourceManager.h"

// Define static member variable
std::unordered_map<std::string, std::unique_ptr<sf::Texture>> ResourceManager::textures;
std::unordered_map<std::string, std::unique_ptr<sf::Font>> ResourceManager::fonts;

// Define static method LoadTexture
void ResourceManager::LoadTexture(const std::string& id, const std::string& filename) {
    std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>();
    if (texture->loadFromFile(filename)) {
        textures[id] = std::move(texture); // No need for RescourceManager::GetTextures()
    }
}

void ResourceManager::LoadFont(const std::string &id, const std::string &filename) {
    std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();
    if (font->loadFromFile(filename)) {
        fonts[id] = std::move(font); // No need for RescourceManager::GetTextures()
    }
}

// Define static method GetTexture
sf::Texture* ResourceManager::GetTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second.get();
    }
    return nullptr;
}

sf::Font *ResourceManager::GetFont(const std::string &id) {
    auto it = fonts.find(id);
    if (it != fonts.end()) {
        return it->second.get();
    }
    return nullptr;
}

// Define static method GetTextures
std::unordered_map<std::string, std::unique_ptr<sf::Texture>>& ResourceManager::GetTextures() {
    return textures;
}

std::unordered_map<std::string, std::unique_ptr<sf::Font>> & ResourceManager::GetFonts() {
    return fonts;
}

