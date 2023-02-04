#include "Boid.hpp"

// For global simulation parameters
#include "BoidManager.hpp"

// For quick vector math
#include "QuickMath.hpp"

Boid::Boid()
{}

Boid::~Boid()
{}

double Boid::distanceTo(const Boid& other)
{
    // Get the radial distance to the other boid's center
    return QuickMath::getMagnitude
        (other.getPosition() - this->getPosition());
}

// TODO(me): Fix bias towards 180°
void Boid::updateRotation()
{
    // Cache this boid's info
    sf::Vector2f currentPos = this->getPosition();
    float currentDirection = this->getRotation();

    // Keep track the driving forces
    sf::Vector2f separationF, allignmentF, cohesionF;

    // Keep track of the average neighbor's position and rotation
    float avgRot = 0;
    sf::Vector2f avgPos = sf::Vector2f(0, 0);

    // Consider all boids that happen to be ranged, in-view neighbors
    // of this one
    size_t consideredNeighbors = 0;
    const Boid* boidArray = BoidManager::getInstance().boidArray;
    const size_t& boidCount = BoidManager::getInstance().boidCount;
    const double& senseRadius = BoidManager::getInstance().senseRadius;

    for (size_t boidIterator = 0; boidIterator < boidCount; ++boidIterator)
    {
        // Don't consider itself as a neighbor
        if (this == &boidArray[boidIterator])
            continue;

        // Check the distance to the current neighbor
        double neighborDist = distanceTo(boidArray[boidIterator]);

        // Only consider it if it doesn't exceed the maximum distance
        // or is at least non-zero
        if (neighborDist > senseRadius || neighborDist == 0)
            continue;
        
        ++consideredNeighbors;

        // Cache neighbor info
        sf::Vector2f neighborPos = boidArray[boidIterator].getPosition();
        float neighborRot = boidArray[boidIterator].getRotation();

        // Update the average position and rotation
        avgPos += neighborPos;
        avgRot += neighborRot;

        // Calculate the neighbor's offset for the separation force
        // And make it inversely proportional to the distance to the neighbor
        sf::Vector2f offset = currentPos - neighborPos;
        offset.x /= neighborDist; offset.y /= neighborDist;

        separationF += offset;
    }

    if (consideredNeighbors == 0)
        return;

    // Compute averages
    avgPos.x /= consideredNeighbors;
    avgPos.y /= consideredNeighbors;
    avgRot /= consideredNeighbors;

    // Compute forces
    separationF.x /= consideredNeighbors;
    separationF.y /= consideredNeighbors;
    separationF *= BoidManager::getInstance().separationC;

    allignmentF.x = sin(avgRot);
    allignmentF.x = cos(avgRot);
    separationF *= BoidManager::getInstance().allignmentC;

    cohesionF = avgPos - currentPos;
    cohesionF *= BoidManager::getInstance().cohesionC;

    // Compute desired direction as a vector sum of the forces,
    // then onto degrees
    float desiredDirection = QuickMath::getDegrees
        (separationF + allignmentF + cohesionF);

    // Compute the turning step and positive angle offset from
    // the desired direction
    double turnStep = 
        BoidManager::getInstance().turnSpeed;
    
    double directionOffset = desiredDirection - currentDirection;
    directionOffset = QuickMath::modulus(directionOffset, 0.0, 360.0);

    // Rotate forward or backward acordingly
    if (directionOffset > 180.0) turnStep *= -1;

    // Go forward
    this->setRotation
    (
        QuickMath::clampAdvance
        (currentDirection, desiredDirection, turnStep, std::function(QuickMath::differenceIsSignificant<float>))
    );
}

void Boid::updatePosition()
{
    // Cache boid info
    sf::Vector2f currentPos = this->getPosition();
    float currentDir = this->getRotation();

    // Update cache'd position via offset
    float radians = QuickMath::degreesToRadians(currentDir);
    currentPos += 
        sf::Vector2f(cos(radians), sin(radians)) * BoidManager::getInstance().flySpeed;

    // Re-adjust position if out of bounds
    const sf::FloatRect& bounds = BoidManager::getInstance().bounds;
    if (currentPos.x < bounds.left)
    {
        float offset = bounds.left - currentPos.x;
        if (offset < 0) offset *= 1;

        currentPos.x = (bounds.left + bounds.width) - offset;
    }
    else if (currentPos.x > bounds.left + bounds.width)
    {
        float offset = (bounds.left + bounds.width) - currentPos.x;
        if (offset < 0) offset *= 1;

        currentPos.x = bounds.left + offset;
    }

    if (currentPos.y < bounds.top)
    {
        float offset = bounds.top - currentPos.y;
        currentPos.y = (bounds.top + bounds.height) - offset;
    }
    else if (currentPos.y > bounds.top + bounds.height)
    {
        float offset = currentPos.y - (bounds.top + bounds.height);
        currentPos.y = bounds.top + offset;
    }

    // Update real-position
    this->setPosition(currentPos);
}
