//
// Created by wouter on 19-2-2024.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include "StateManager.h"

class StateManager;

struct Context {
    std::unique_ptr<StateManager> state_manager;
    std::unique_ptr<sf::RenderWindow> window;

    Context() {
        state_manager = std::make_unique<StateManager>();
        window = std::make_unique<sf::RenderWindow>();
    }
};


class Application {
private:
    std::shared_ptr<Context> context;
    const sf::Time TIME_PER_SECOND = sf::seconds(1.f/60.f);
    sf::Clock clock;

public:

    Application();
    ~Application();

    void Run();

};

#endif //APPLICATION_H
