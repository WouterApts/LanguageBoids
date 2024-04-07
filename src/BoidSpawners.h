//
// Created by wouter on 20-3-2024.
//

#ifndef BOIDSPAWNERS_H
#define BOIDSPAWNERS_H
#include <memory>
#include <SFML/Graphics.hpp>
#include "World.h"
#include "Boid.h"


class CircularSpawner {
public:
    Eigen::Vector2f center_pos;
    float radius;

    CircularSpawner(Eigen::Vector2f center_pos, float radius);
    virtual ~CircularSpawner() = default;
    Eigen::Vector2f CalcRandomPointInSpawnZone(const World &world, float buffer = 0.f) const;
    virtual bool IsInside(Eigen::Vector2f pos, float radius);
};

class RectangularSpawner {
public:
    Eigen::Vector2f pos;
    float width;
    float height;

    RectangularSpawner(Eigen::Vector2f pos, float width, float height);
    virtual ~RectangularSpawner() = default;
    Eigen::Vector2f CalcRandomPointInSpawnZone(const World &world, float buffer = 0.f) const;

    virtual bool IsInside(Eigen::Vector2f pos, float radius);
};

class BoidSpawner {
public:
    virtual ~BoidSpawner() = default;
    virtual bool IsInside(Eigen::Vector2f pos, float radius) { return false; }
    virtual void Draw(sf::RenderWindow *window) const {}
    virtual std::string ToString() {return "Error";};
};

class KeyBoidSpawner : public BoidSpawner {
public:
    int boids_spawned;
    int language_key;
    sf::Text text;

    KeyBoidSpawner(int boids_spawned, int language_key);
    virtual void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<KeyBoid>> &boids) {};
};


class KeyBoidCircularSpawner : public KeyBoidSpawner, public CircularSpawner {
public:
    sf::CircleShape circle;

    KeyBoidCircularSpawner(int boids_spawned, int language_key, Eigen::Vector2f center_pos, float radius);
    void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<KeyBoid>> &boids) override;
    void Draw(sf::RenderWindow* window) const override;
    bool IsInside(Eigen::Vector2f pos, float radius) override;

    std::string ToString() override;
    static std::shared_ptr<KeyBoidCircularSpawner> FromString(const std::string &str);
};


class KeyBoidRectangularSpawner : public KeyBoidSpawner, public RectangularSpawner {
public:
    sf::RectangleShape rect;

    KeyBoidRectangularSpawner(int boids_spawned, int language_key, Eigen::Vector2f pos, float width, float height);
    void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<KeyBoid>> &boids) override;
    void Draw(sf::RenderWindow* window) const override;
    bool IsInside(Eigen::Vector2f pos, float radius) override;

    std::string ToString() override;
    static std::shared_ptr<KeyBoidRectangularSpawner> FromString(const std::string &str);
};


#endif //BOIDSPAWNERS_H
