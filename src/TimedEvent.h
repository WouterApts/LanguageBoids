//
// Created by wouter on 19-3-2024.
//

#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H


#include <SFML/System.hpp>
#include <functional>

class TimedEvent {
public:
    TimedEvent() : m_duration(sf::seconds(0.f)), m_running(false) {}

    void start(const sf::Time& duration, const std::function<void()>& callback) {
        m_duration = duration;
        m_callback = callback;
        m_clock.restart();
        m_running = true;
    }

    void update() {
        if (m_running && m_clock.getElapsedTime() >= m_duration) {
            if (m_callback) {
                m_callback();
            }
            m_running = false;
        }
    }

    bool isRunning() const {
        return m_running;
    }

private:
    sf::Clock m_clock;
    sf::Time m_duration;
    std::function<void()> m_callback;
    bool m_running;
};

#endif //TIMEDEVENT_H
