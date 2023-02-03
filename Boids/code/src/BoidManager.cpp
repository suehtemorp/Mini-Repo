#include "BoidManager.hpp"

// Constructor & Destructor

BoidManager::BoidManager() :
texture(new sf::Texture()), boidScale(1),
boidCount(0), boidArray(nullptr),
flySpeed(1), senseRadius(1), turnSpeed(90),
separationC(1), allignmentC(1), cohesionC(1)
{}

BoidManager::~BoidManager()
{
    delete boidArray;
    delete texture;
}

// Singleton access

const BoidManager& BoidManager::getInstance()
{return static_cast<const BoidManager&>(accessInstance());}

BoidManager& BoidManager::accessInstance() 
{
    static std::unique_ptr<BoidManager> staticInstance(new BoidManager());
    return *staticInstance;
}

// Setters

bool BoidManager::setTexture(const std::string& texture_path)
{
    if(this->texture->loadFromFile(texture_path))
    {
        this->resetBoidsTexture();
        return true;
    }
    else 
        return false;
}

bool BoidManager::setBounds(const sf::FloatRect& screen_bounds)
{
    if (screen_bounds.width == 0 || screen_bounds.height == 0)
        return false;

    else
    {
        this->bounds = screen_bounds;
        return true;
    }
    
}

bool BoidManager::setBoidCount(const size_t boid_count)
{
    if (boid_count == 0)
        return false;
    
    // Discard all previous boids and create new ones
    delete this->boidArray;
    this->boidCount = boid_count; this->boidArray = new Boid[boidCount];

    // Reset every boid's texture and transform origin
    this->resetBoidsTexture();

    // Give each a random starting position and rotation
    sf::Vector2f randPos;
    float randDir;
    for (size_t boidIter = 0; boidIter < this->boidCount; ++boidIter)
    {
        randPos.x = this->bounds.left + (rand() % (unsigned)(this->bounds.width)); 
        randPos.y = this->bounds.top + (rand() % (unsigned)(this->bounds.height));
        randDir = rand() % 360;

        this->boidArray[boidIter].setPosition(randPos);
        this->boidArray[boidIter].setRotation(randDir);
    }

    return true;
}

// Rendering and simulation updating

void BoidManager::update()
{
    // Move every boid first
    for (size_t boidIter = 0; boidIter < boidCount; ++boidIter)
        boidArray[boidIter].updatePosition();
    
    // Update each and every boid's momentum
    for (size_t boidIter = 0; boidIter < boidCount; ++boidIter)
        boidArray[boidIter].updateRotation();
}

void BoidManager::resetBoidsTexture()
{
    for (size_t boidIter = 0; boidIter < boidCount; ++boidIter)
    {
        boidArray[boidIter].setTexture(*(this->texture), true);
        boidArray[boidIter].setScale(sf::Vector2f(1,1) * this->boidScale);

        sf::FloatRect localBounds = boidArray[boidIter].getLocalBounds();
        this->boidArray[boidIter].
            setOrigin(localBounds.width / 2.f, localBounds.height / 2.f);
    }
}

void BoidManager::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (size_t boidIter = 0; boidIter < boidCount; ++boidIter)
        target.draw(boidArray[boidIter], states);
}

void BoidManager::freeTexture()
{
    delete this->texture;
    this->texture = nullptr;
}
