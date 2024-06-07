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
    sf::CircleShape circle;

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
    sf::RectangleShape rect;

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


//----------------------------------//
//         KEY BOID SPAWNERS        //
//----------------------------------//
class CompBoidSpawner : public BoidSpawner {
public:
    int boids_spawned;
    int language_key;
    sf::Text text;

    CompBoidSpawner(int boids_spawned, int language_key);
    virtual void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<CompBoid>> &boids) {};

    void SetTextString();
};


class CompBoidCircularSpawner : public CompBoidSpawner, public CircularSpawner {
public:

    CompBoidCircularSpawner(int boids_spawned, int language_key, Eigen::Vector2f center_pos, float radius);
    void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<CompBoid>> &boids) override;
    void Draw(sf::RenderWindow* window) const override;
    bool IsInside(Eigen::Vector2f pos, float radius) override;

    std::string ToString() override;
    static std::shared_ptr<CompBoidCircularSpawner> FromString(const std::string &str);
};


class CompBoidRectangularSpawner : public CompBoidSpawner, public RectangularSpawner {
public:
    CompBoidRectangularSpawner(int boids_spawned, int language_key, Eigen::Vector2f pos, float width, float height);
    void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<CompBoid>> &boids) override;
    void Draw(sf::RenderWindow* window) const override;
    bool IsInside(Eigen::Vector2f pos, float radius) override;

    std::string ToString() override;
    static std::shared_ptr<CompBoidRectangularSpawner> FromString(const std::string &str);
};

//----------------------------------//
//       VECTOR BOID SPAWNERS       //
//----------------------------------//
class EvoBoidSpawner : public BoidSpawner {
public:
    int boids_spawned;
    int vector_seed;
    float feature_bias;
    sf::Text text;

    EvoBoidSpawner(int boids_spawned, int vector_seed, float feature_bias);
    Eigen::VectorXi GetRandomLanguageVector(const std::shared_ptr<SimulationConfig> &config) const;
    virtual void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<EvoBoid>> &boids) {};

    void SetTextString();

};

class EvoBoidCircularSpawner : public EvoBoidSpawner, public CircularSpawner {
public:

    EvoBoidCircularSpawner(int boids_spawned, int vector_seed, float feature_bias, Eigen::Vector2f center_pos, float radius);
    void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<EvoBoid>> &boids) override;
    void Draw(sf::RenderWindow* window) const override;
    bool IsInside(Eigen::Vector2f pos, float radius) override;

    std::string ToString() override;
    static std::shared_ptr<EvoBoidCircularSpawner> FromString(const std::string &str);
};


class EvoBoidRectangularSpawner : public EvoBoidSpawner, public RectangularSpawner {
public:

    EvoBoidRectangularSpawner(int boids_spawned, int vector_seed, float feature_bias, const Eigen::Vector2f &pos, float width, float height);
    void AddBoids(const World &world, const std::shared_ptr<SimulationConfig> &config, std::vector<std::shared_ptr<EvoBoid>> &boids) override;
    void Draw(sf::RenderWindow* window) const override;
    bool IsInside(Eigen::Vector2f pos, float radius) override;

    std::string ToString() override;
    static std::shared_ptr<EvoBoidRectangularSpawner> FromString(const std::string &str);
};


#endif //BOIDSPAWNERS_H
