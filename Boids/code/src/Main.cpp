// TODO(me): Heavy refactoring

#include "BoidManager.hpp"

int main()
{
    // Create OpenGL context first via SFML window creation
    sf::RenderWindow window(sf::VideoMode(900, 700), "Boid Sim v1");

    // Setup essential boid render resources
    BoidManager::accessInstance().setTexture("./res/galaga.png");
    BoidManager::accessInstance().boidScale = 0.05;

    // Setup boid play rules
    BoidManager::accessInstance().setBounds
        (sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    BoidManager::accessInstance().flySpeed = 0.25;
    BoidManager::accessInstance().turnSpeed = 0.15;
    BoidManager::accessInstance().senseRadius = 10;

    BoidManager::accessInstance().cohesionC = 10;
    BoidManager::accessInstance().allignmentC = 20;
    BoidManager::accessInstance().separationC = 30;

    // Setup boid count
    BoidManager::accessInstance().setBoidCount(100);

    // Run the simulation as long as the window is open
    while (window.isOpen())
    {
        // Update one step the simulation
        BoidManager::accessInstance().update();

        // Merely process events in the case of window closing
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the screen with black
        window.clear(sf::Color::Black);

        // Draw the simulation
        window.draw(BoidManager::getInstance());

        // Display the fram
        window.display();
    }

    // Free the current texture while still on the SFML OpenGL
    // context
    BoidManager::accessInstance().freeTexture();

    return 0;
}