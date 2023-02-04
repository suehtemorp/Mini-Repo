#ifndef BOID_HPP
#define BOID_HPP

// For movement and rendering
#include <SFML/Graphics.hpp>

class Boid : public sf::Sprite
{
    private:
        float nextDir;
        bool updatedDir;
        double distanceTo(const Boid& other);

    public:
        Boid();
        ~Boid();

        void updateRotation();
        void updatePosition();
};

#endif
