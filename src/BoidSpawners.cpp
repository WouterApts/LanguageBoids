//
// Created by wouter on 20-3-2024.
//

#include "BoidSpawners.h"

#include <iostream>
#include <utility>

#include "LanguageManager.h"
#include "ResourceManager.h"
#include "Utility.h"

CircularSpawner::CircularSpawner(Eigen::Vector2f center_pos, float radius) : center_pos(std::move(center_pos)), radius(radius) {}

Eigen::Vector2f CircularSpawner::CalcRandomPointInSpawnZone(const World& world) {

    float angle = GetRandomFloatBetween(0, 2*std::numbers::pi);
    float length = GetRandomFloatBetween(0, radius);
    float x = center_pos.x() + std::cos(angle) * length;
    float y = center_pos.y() + std::sin(angle) * length;

    //Keep spawn position within world border
    x = std::max(std::min(x, world.width), 0.f);
    y = std::max(std::min(y, world.height), 0.f);

    return {x, y};
}

RectangularSpawner::RectangularSpawner(Eigen::Vector2f pos, float width, float height)
    : pos(std::move(pos)), width(width), height(height) {
}

bool RectangularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return pos.x() + radius > this->pos.x() && pos.x() - radius < this->pos.x() + width &&
           pos.y() + radius > this->pos.y() && pos.y() - radius < this->pos.y() + height;
}

CompBoidSpawner::CompBoidSpawner(int boids_spawned, int language_key) : boids_spawned(boids_spawned), language_key(language_key) {}

CompBoidCircularSpawner::CompBoidCircularSpawner(int boids_spawned, int language_key, Eigen::Vector2f center_pos, float radius)
    : CompBoidSpawner(boids_spawned, language_key), CircularSpawner(std::move(center_pos), radius) {

    sf::Color color = LanguageManager::GetLanguageColor(language_key);
    sf::Font& font = *ResourceManager::GetFont("arial");
    text.setString(std::to_string(boids_spawned));
    text.setFillColor(color);
    text.setCharacterSize(static_cast<int>(std::max(std::min(radius/2, 500.f), 100.f)));
    text.setFont(font);
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = this->center_pos.x() - textBounds.width / 2.0f;
    float textPosY = this->center_pos.y() - textBounds.height;
    // Set the position of the text
    text.setPosition(textPosX, textPosY);

    circle.setRadius(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(this->center_pos.x(), this->center_pos.y());

    color.a = 100;
    circle.setFillColor(color);
}

void CompBoidCircularSpawner::AddBoids(const World& world, std::vector<std::shared_ptr<CompBoid>>& boids) {
    for (int i = 0; i < boids_spawned; ++i) {
        auto spawn_point = CalcRandomPointInSpawnZone(world);
        auto new_boid = std::make_shared<CompBoid>(spawn_point, Eigen::Vector2f::Zero(), Eigen::Vector2f::Zero(), language_key);
        boids.emplace_back(std::move(new_boid));
    }
}

bool CircularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return (center_pos - pos).norm() < this->radius + radius;
}

void CompBoidCircularSpawner::Draw(sf::RenderWindow *window) const {
    window->draw(circle);
    window->draw(text);
}

bool CompBoidCircularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return CircularSpawner::IsInside(pos, radius);
}

std::string CompBoidCircularSpawner::ToString() {
    std::stringstream ss;
    ss << "CompBoidCircularSpawner: "
       << "Center: " << center_pos.x() << " , " << center_pos.y() << " "
       << "Radius: " << radius << " "
       << "Boids: " << boids_spawned << " "
       << "Language: " << language_key;
    return ss.str();
}

std::shared_ptr<CompBoidCircularSpawner> CompBoidCircularSpawner::FromString(const std::string& str) {
    std::istringstream iss(str);
    std::string type;
    std::string delimiter;
    Eigen::Vector2f center;
    float radius;
    int language_key;
    int boids_spawned;

    if (!(iss >> type >> delimiter >> center.x() >> delimiter >> center.y() >> delimiter
              >> radius >> delimiter >> boids_spawned >> delimiter >> language_key)) {
                std::cerr << "Error: Invalid CircleObstacle string format: " << str << std::endl;
                return nullptr;
              }
    return std::make_shared<CompBoidCircularSpawner>(boids_spawned, language_key, center, radius);
}


CompBoidRectangularSpawner::CompBoidRectangularSpawner(int boids_spawned, int language_key, Eigen::Vector2f pos, float width, float height)
    : CompBoidSpawner(boids_spawned, language_key), RectangularSpawner(pos, width, height) {

    sf::Color color = LanguageManager::GetLanguageColor(language_key);
    sf::Font& font = *ResourceManager::GetFont("arial");
    text.setString(std::to_string(boids_spawned));
    text.setFillColor(color);
    text.setCharacterSize(static_cast<int>(std::max(std::min(width/4, 500.f), 100.f)));
    text.setFont(font);

    rect.setSize(sf::Vector2f(width, height));
    rect.setPosition(sf::Vector2f(pos.x(), pos.y()));

    color.a = 100;
    rect.setFillColor(color);

    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = this->pos.x() + this->rect.getSize().x/2 - textBounds.width / 2.0f;
    float textPosY = this->pos.y() + this->rect.getSize().y/2 - textBounds.height;
    // Set the position of the text
    std::cout << textPosX << " " << textPosY;
    text.setPosition(textPosX, textPosY);
}

void CompBoidRectangularSpawner::AddBoids(const World &world, std::vector<std::shared_ptr<CompBoid>> &boids) {
    CompBoidSpawner::AddBoids(world, boids);
}

void CompBoidRectangularSpawner::Draw(sf::RenderWindow *window) const {
    window->draw(rect);
    window->draw(text);
}

bool CompBoidRectangularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return RectangularSpawner::IsInside(pos, radius);
}

std::string CompBoidRectangularSpawner::ToString() {
    std::stringstream ss;
    ss << "CompBoidCircularSpawner: "
       << "Position: " << pos.x() << " , " << pos.y() << " "
       << "Width: " << width << " "
       << "height: " << height << " "
       << "Boids: " << boids_spawned << " "
       << "Language: " << language_key;
    return ss.str();
}
