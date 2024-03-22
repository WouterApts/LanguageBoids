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
    Eigen::Vector2f CalcRandomPointInSpawnZone(const World &world);
    virtual bool IsInside(Eigen::Vector2f pos, float radius);
};

class RectangularSpawner {
public:


    Eigen::Vector2f pos;
    float width;
    float height;

    RectangularSpawner(Eigen::Vector2f pos, float width, float height);
    virtual ~RectangularSpawner() = default;
    Eigen::Vector2f CalcRandomPointInSpawnZone();

    virtual bool IsInside(Eigen::Vector2f pos, float radius);
};

class CompBoidSpawner {
public:
    int boids_spawned;
    int language_key;
    sf::CircleShape circle;
    sf::Text text;

    CompBoidSpawner(int boids_spawned, int language_key);
    virtual ~CompBoidSpawner() = default;
    virtual void AddBoids(const World &world, std::vector<std::shared_ptr<CompBoid>> &boids) {};
    virtual void Draw(sf::RenderWindow *window) const {}
    virtual bool IsInside(Eigen::Vector2f pos, float radius) {return false;};
    virtual std::string ToString() {return "Error";};
};


class CompBoidCircularSpawner : public CompBoidSpawner, public CircularSpawner {
public:
    CompBoidCircularSpawner(int boids_spawned, int language_key, Eigen::Vector2f center_pos, float radius);
    void AddBoids(const World &world, std::vector<std::shared_ptr<CompBoid>> &boids) override;
    void Draw(sf::RenderWindow* window) const override;
    bool IsInside(Eigen::Vector2f pos, float radius) override;

    std::string ToString() override;
    static std::shared_ptr<CompBoidCircularSpawner> FromString(const std::string &str);
};


class CompBoidRectangularSpawner : public CompBoidSpawner, public RectangularSpawner {
public:
    CompBoidRectangularSpawner(int boids_spawned, int language_key, Eigen::Vector2f pos, float width, float height);
    void AddBoids(const World &world, std::vector<std::shared_ptr<CompBoid>> &boids) override;
    void Draw(sf::RenderWindow* window) const override;
    bool IsInside(Eigen::Vector2f pos, float radius) override;
    std::string ToString() override;
};


#endif //BOIDSPAWNERS_H
