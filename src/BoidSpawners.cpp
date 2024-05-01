//
// Created by wouter on 20-3-2024.
//

#include "BoidSpawners.h"

#include <iostream>
#include <random>
#include <utility>

#include "LanguageManager.h"
#include "ResourceManager.h"
#include "Utility.h"

CircularSpawner::CircularSpawner(Eigen::Vector2f center_pos, float radius) : center_pos(std::move(center_pos)), radius(radius) {
    circle.setRadius(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(this->center_pos.x(), this->center_pos.y());
}

Eigen::Vector2f CircularSpawner::CalcRandomPointInSpawnZone(const World& world, float buffer) const {

    float angle = GetRandomFloatBetween(0, 2*std::numbers::pi);
    float length = GetRandomFloatBetween(0, radius);
    float x = center_pos.x() + std::cos(angle) * length;
    float y = center_pos.y() + std::sin(angle) * length;

    //Keep spawn position within world border (including buffer)
    x = std::max(std::min(x, world.width - buffer), buffer);
    y = std::max(std::min(y, world.height - buffer), buffer);

    return {x, y};
}


RectangularSpawner::RectangularSpawner(Eigen::Vector2f pos, float width, float height)
    : pos(std::move(pos)), width(width), height(height) {
    rect.setSize(sf::Vector2f(width, height));
    rect.setPosition(sf::Vector2f(pos.x(), pos.y()));
}

Eigen::Vector2f RectangularSpawner::CalcRandomPointInSpawnZone(const World &world, float buffer) const {
    float x = pos.x() + GetRandomFloatBetween(0, width);
    float y = pos.y() + GetRandomFloatBetween(0, height);

    //Keep spawn position within world border (including buffer)
    x = std::max(std::min(x, world.width - buffer), buffer);
    y = std::max(std::min(y, world.height - buffer), buffer);

    return {x, y};
}

bool RectangularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return pos.x() + radius > this->pos.x() && pos.x() - radius < this->pos.x() + width &&
           pos.y() + radius > this->pos.y() && pos.y() - radius < this->pos.y() + height;
}

//----------------------------------//
//         KEY BOID SPAWNERS        //
//----------------------------------//
KeyBoidSpawner::KeyBoidSpawner(int boids_spawned, int language_key) : boids_spawned(boids_spawned), language_key(language_key) {}

void KeyBoidSpawner::SetTextString() {
    sf::Color color = LanguageManager::GetLanguageColor(language_key);
    sf::Font& font = *ResourceManager::GetFont("arial");
    text.setString(std::to_string(boids_spawned));
    text.setFillColor(color);
    text.setFont(font);
}

KeyBoidCircularSpawner::KeyBoidCircularSpawner(int boids_spawned, int language_key, Eigen::Vector2f center_pos, float radius)
    : KeyBoidSpawner(boids_spawned, language_key), CircularSpawner(std::move(center_pos), radius) {

    SetTextString();

    // Position text in the circle
    sf::Color color = LanguageManager::GetLanguageColor(language_key);
    text.setCharacterSize(static_cast<int>(std::max(std::min(radius/2, 500.f), 100.f)));
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = this->center_pos.x() - textBounds.width / 2.0f;
    float textPosY = this->center_pos.y() - textBounds.height;
    text.setPosition(textPosX, textPosY);

    color.a = 100;
    circle.setFillColor(color);
}

void KeyBoidCircularSpawner::AddBoids(const World& world, const std::shared_ptr<SimulationConfig>& config, std::vector<std::shared_ptr<KeyBoid>>& boids) {
    for (int i = 0; i < boids_spawned; ++i) {
        auto spawn_point = CalcRandomPointInSpawnZone(world, config->BOID_COLLISION_RADIUS);
        auto new_boid = std::make_shared<KeyBoid>(spawn_point, Eigen::Vector2f::Zero(), Eigen::Vector2f::Zero(), config, language_key);
        boids.emplace_back(std::move(new_boid));
    }
}

bool CircularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return (center_pos - pos).norm() < this->radius + radius;
}

void KeyBoidCircularSpawner::Draw(sf::RenderWindow *window) const {
    window->draw(circle);
    if (boids_spawned > 0) {
        window->draw(text);
    }
}

bool KeyBoidCircularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return CircularSpawner::IsInside(pos, radius);
}

std::string KeyBoidCircularSpawner::ToString() {
    std::stringstream ss;
    ss << "KeyBoidCircularSpawner: "
       << "Center: " << center_pos.x() << " , " << center_pos.y() << " "
       << "Radius: " << radius << " "
       << "Boids: " << boids_spawned << " "
       << "Language: " << language_key;
    return ss.str();
}

std::shared_ptr<KeyBoidCircularSpawner> KeyBoidCircularSpawner::FromString(const std::string& str) {
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
    return std::make_shared<KeyBoidCircularSpawner>(boids_spawned, language_key, center, radius);
}


KeyBoidRectangularSpawner::KeyBoidRectangularSpawner(int boids_spawned, int language_key, Eigen::Vector2f pos, float width, float height)
    : KeyBoidSpawner(boids_spawned, language_key), RectangularSpawner(pos, width, height) {

    SetTextString();

    // Position the text in the rectangle
    sf::Color color = LanguageManager::GetLanguageColor(language_key);
    text.setCharacterSize(static_cast<int>(std::max(std::min(width/4, 500.f), 100.f)));
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = this->pos.x() + this->rect.getSize().x/2 - textBounds.width / 2.0f;
    float textPosY = this->pos.y() + this->rect.getSize().y/2 - textBounds.height;
    text.setPosition(textPosX, textPosY);

    color.a = 100;
    rect.setFillColor(color);
}

void KeyBoidRectangularSpawner::AddBoids(const World& world, const std::shared_ptr<SimulationConfig>& config, std::vector<std::shared_ptr<KeyBoid>>& boids) {
    for (int i = 0; i < boids_spawned; ++i) {
        auto spawn_point = CalcRandomPointInSpawnZone(world, config->BOID_COLLISION_RADIUS);
        auto new_boid = std::make_shared<KeyBoid>(spawn_point, Eigen::Vector2f::Zero(), Eigen::Vector2f::Zero(), config, language_key);
        boids.emplace_back(std::move(new_boid));
    }
}

void KeyBoidRectangularSpawner::Draw(sf::RenderWindow *window) const {
    window->draw(rect);
    if (boids_spawned > 0) {
        window->draw(text);
    }
}

bool KeyBoidRectangularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return RectangularSpawner::IsInside(pos, radius);
}

std::string KeyBoidRectangularSpawner::ToString() {
    std::stringstream ss;
    ss << "KeyBoidRectangularSpawner: "
       << "Position: " << pos.x() << " , " << pos.y() << " "
       << "Width: " << width << " "
       << "height: " << height << " "
       << "Boids: " << boids_spawned << " "
       << "Language: " << language_key;
    return ss.str();
}

std::shared_ptr<KeyBoidRectangularSpawner> KeyBoidRectangularSpawner::FromString(const std::string &str) {
    std::istringstream iss(str);
    std::string type;
    std::string delimiter;
    Eigen::Vector2f pos;
    float width;
    float height;
    int boids_spawned;
    int language_key;

    if (!(iss >> type >> delimiter >> pos.x() >> delimiter >> pos.y() >> delimiter
              >> width >> delimiter >> height >> delimiter >> boids_spawned >> delimiter >> language_key)) {
        std::cerr << "Error: Invalid CircleObstacle string format: " << str << std::endl;
        return nullptr;
              }
    return std::make_shared<KeyBoidRectangularSpawner>(boids_spawned, language_key, pos, width, height);
}

//----------------------------------//
//       VECTOR BOID SPAWNERS       //
//----------------------------------//
VectorBoidSpawner::VectorBoidSpawner(int boids_spawned, int vector_seed, float feature_bias)
    : boids_spawned(boids_spawned), vector_seed(vector_seed), feature_bias(std::min(feature_bias, 1.f)) {}

void VectorBoidSpawner::SetTextString() {
    sf::Color color = sf::Color::White;
    sf::Font& font = *ResourceManager::GetFont("arial");
    auto text_str = "\n boids: " + std::to_string(boids_spawned) + "\n seed: " + std::to_string(vector_seed) + "\n bias: " + std::to_string(feature_bias).substr(0,3);
    text.setString(text_str);
    text.setFillColor(color);
    text.setFont(font);
}

VectorBoidCircularSpawner::VectorBoidCircularSpawner(int boids_spawned, int vector_seed, float feature_bias, Eigen::Vector2f center_pos, float radius)
    : VectorBoidSpawner(boids_spawned, vector_seed, feature_bias), CircularSpawner(std::move(center_pos), radius) {

    SetTextString();

    // position text in circle
    sf::Color color = sf::Color::White;
    text.setCharacterSize(static_cast<int>(std::max(std::min(radius/4, 500.f), 50.f)));
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = this->center_pos.x() - textBounds.width / 2.0f;
    float textPosY = this->center_pos.y() - textBounds.height;
    text.setPosition(textPosX, textPosY);

    color.a = 100;
    circle.setFillColor(color);
}

Eigen::VectorXi VectorBoidSpawner::GetRandomLanguageVector(const std::shared_ptr<SimulationConfig> &config) const{
    // Initialize bit-string language vector based on seed
    std::mt19937 rng(vector_seed); // Mersenne Twister random number engine with seed
    std::binomial_distribution<int> distribution(1, feature_bias);
    Eigen::VectorXi language_vector(config->LANGUAGE_SIZE);
    for (int i = 0; i < config->LANGUAGE_SIZE; ++i) {
        language_vector(i) = distribution(rng);
    }
    return language_vector;
}

void VectorBoidCircularSpawner::AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config,
                                         std::vector<std::shared_ptr<VectorBoid>> &boids) {

    Eigen::VectorXi language_vector = GetRandomLanguageVector(config);
    for (int i = 0; i < boids_spawned; ++i) {
        auto spawn_point = CalcRandomPointInSpawnZone(world, config->BOID_COLLISION_RADIUS);
        auto new_boid = std::make_shared<VectorBoid>(spawn_point, Eigen::Vector2f::Zero(), Eigen::Vector2f::Zero(), config,
                                                     language_vector, 1);
        boids.emplace_back(std::move(new_boid));
    }
}

void VectorBoidCircularSpawner::Draw(sf::RenderWindow *window) const {
    window->draw(circle);
    if (boids_spawned > 0) {
        window->draw(text);
    }
}

bool VectorBoidCircularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
    return CircularSpawner::IsInside(pos, radius);
}

std::string VectorBoidCircularSpawner::ToString() {
    std::stringstream ss;
    ss << "VectorBoidCircularSpawner: "
       << "Center: " << center_pos.x() << " , " << center_pos.y() << " "
       << "Radius: " << radius << " "
       << "Boids: " << boids_spawned << " "
       << "Seed: " << vector_seed << " "
       << "Bias: " << feature_bias << " ";
    return ss.str();
}

std::shared_ptr<VectorBoidCircularSpawner> VectorBoidCircularSpawner::FromString(const std::string &str) {
    std::istringstream iss(str);
    std::string type;
    std::string delimiter;
    Eigen::Vector2f center;
    float radius;
    float feature_bias;
    int vector_seed;
    int boids_spawned;

    if (!(iss >> type >> delimiter >> center.x() >> delimiter >> center.y() >> delimiter
              >> radius >> delimiter >> boids_spawned >> delimiter >> vector_seed >> delimiter >> feature_bias)) {
                std::cerr << "Error: Invalid CircleObstacle string format: " << str << std::endl;
                return nullptr;
              }
    return std::make_shared<VectorBoidCircularSpawner>(boids_spawned, vector_seed, feature_bias, center, radius);
}

VectorBoidRectangularSpawner::VectorBoidRectangularSpawner(int boids_spawned, int vector_seed, float feature_bias, const Eigen::Vector2f &pos, float width, float height)
    : VectorBoidSpawner(boids_spawned, vector_seed, feature_bias), RectangularSpawner(pos, width, height) {

    SetTextString();

    // position text in the rectangle
    sf::Color color = sf::Color::White;
    sf::FloatRect textBounds = text.getLocalBounds();
    float textPosX = this->pos.x() + this->rect.getSize().x/2 - textBounds.width / 2.0f;
    float textPosY = this->pos.y() + this->rect.getSize().y/2 - textBounds.height;
    text.setPosition(textPosX, textPosY);

    color.a = 100;
    rect.setFillColor(color);
}

void VectorBoidRectangularSpawner::AddBoids(const World& world, const std::shared_ptr<SimulationConfig>& config, std::vector<std::shared_ptr<VectorBoid>>& boids) {

    Eigen::VectorXi language_vector = GetRandomLanguageVector(config);
    for (int i = 0; i < boids_spawned; ++i) {
        auto spawn_point = CalcRandomPointInSpawnZone(world, config->BOID_COLLISION_RADIUS);
        auto new_boid = std::make_shared<VectorBoid>(spawn_point, Eigen::Vector2f::Zero(), Eigen::Vector2f::Zero(), config,
                                                     language_vector, 1);
        boids.emplace_back(std::move(new_boid));
    }
}

void VectorBoidRectangularSpawner::Draw(sf::RenderWindow *window) const {
    VectorBoidSpawner::Draw(window);
}

bool VectorBoidRectangularSpawner::IsInside(Eigen::Vector2f pos, float radius) {
}

std::string VectorBoidRectangularSpawner::ToString() {
    return VectorBoidSpawner::ToString();
}

std::shared_ptr<VectorBoidCircularSpawner> VectorBoidRectangularSpawner::FromString(const std::string &str) {
}

