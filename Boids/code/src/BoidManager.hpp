#ifndef BOID_MANAGER_HPP
#define BOID_MANAGER_HPP

// For rendering
#include <SFML/Graphics.hpp>

// For Boids
#include "Boid.hpp"

// Forward declaration to avoid circular dependency
class Boid;

class BoidManager : public sf::Drawable
{
    public:
        // Rendering parameters
        float boidScale;

        // Simulation parameters
        sf::FloatRect bounds;
        float turnSpeed;
        float flySpeed;
        float senseRadius;

        float separationC;
        float allignmentC;
        float cohesionC;

        // Boid collection
        size_t boidCount;
        Boid* boidArray;

        // Forbid any copy-construction or copy-assignment
        BoidManager(BoidManager const&) = delete;
        BoidManager& operator=(BoidManager const&) = delete;

        // Get read-only access to current instance
        static const BoidManager& getInstance();

        // Get read-write access to current instance
        static BoidManager& accessInstance();

        // Set the current texture for boids
        bool setTexture(const std::string& texture_path);

        // Set the screen bounds for boids
        bool setBounds(const sf::FloatRect& screen_bounds);

        // Set the current boid count
        bool setBoidCount(const size_t boid_count);

        // Update the simulation
        void update();

        // Draw all boids
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        // Free the current texture
        void freeTexture();

        // Destroy BoidManager and free its associated resources
        ~BoidManager();

    private:

        // Shared rendering resources
        sf::Texture* texture;

        // Default-construct a BoidManager
        BoidManager();

        // Update boids texture and center after texture swap
        void resetBoidsTexture();
};

#endif
