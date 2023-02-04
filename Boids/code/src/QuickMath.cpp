#include "QuickMath.hpp"

// Implement non-template functions
namespace QuickMath
{
    // Get degrees as radians
    float degreesToRadians(double degrees)
    {
        degrees = modulus(degrees, 0.0, 360.0);
        return (degrees / 180.0) * std::numbers::pi;
    }

    // Get radians as degrees
    float radiansToDegrees(float radians)
    {
        radians = modulus(radians, 0.f, static_cast<float>(std::numbers::pi * 2));
        return (radians / std::numbers::pi) * 180.0;
    }
}
