// TODO(me): Heavy refactoring

#include "BoidManager.hpp"

int main()
{
    // Create OpenGL context first via SFML window creation
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Boid Sim v1");

    // Setup background render resources
    sf::Texture* bgTexture = new sf::Texture();
    bgTexture->loadFromFile("./res/bg.png");

    sf::Sprite bg; bg.setPosition(0,0);
    bg.setTexture(*bgTexture);
    bg.setScale(static_cast<float>(window.getSize().x) / bg.getLocalBounds().width
        , static_cast<float>(window.getSize().y) / bg.getLocalBounds().height);

    // Setup essential boid render resources
    BoidManager::accessInstance().setTexture("./res/galaga.png");
    BoidManager::accessInstance().boidScale = 0.05;

    // Setup boid play rules
    BoidManager::accessInstance().setBounds
        (sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    BoidManager::accessInstance().flySpeed = 0.4;
    BoidManager::accessInstance().turnSpeed = 0.2;
    BoidManager::accessInstance().senseRadius = 50;

    BoidManager::accessInstance().cohesionC = 1;
    BoidManager::accessInstance().allignmentC = 2;
    BoidManager::accessInstance().separationC = 1;

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

        // Draw the background
        window.draw(bg);

        // Draw the simulation
        window.draw(BoidManager::getInstance());

        // Display the frame
        window.display();
    }

    // Free the boid texture and bg texture while still on the SFML OpenGL
    // context
    BoidManager::accessInstance().freeTexture();
    delete bgTexture;

    return 0;
}