#ifndef VECTOR_SHORTCUTS_HPP
#define VECTOR_SHORTCUTS_HPP

// For vector math
#include <SFML/Graphics.hpp>

// For other goodies, and constantes
#include <numbers>

namespace QuickMath
{
    // Get vector magnitude
    template <typename T>
    inline double getMagnitude(const sf::Vector2<T>& vec)
    {
        // Apply Pythagoras' Theorem
        return std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2));
    }

    // Get vector as itself normalized
    template <typename T>
    sf::Vector2<T> getNormalized(const sf::Vector2<T>& vec)
    {
        // Scale the vector back to unit size
        double magnitude = getMagnitude(vec);
        return sf::Vector2<T>(vec.x / magnitude, vec.y / magnitude);
    }

    // Get degrees as radians
    float degreesToRadians(double degrees)
    {
        while (degrees < 0) degrees += 360.0;
        return (degrees / 180.0) * std::numbers::pi;
    }

    // Get radians as degrees
    float radiansToDegrees(float radians)
    {
        while (radians < 0) radians += (std::numbers::pi * 2);
        return (radians / std::numbers::pi) * 180.0;
    }

    // Get orientation of a vector in polar notation, as degrees
    template <typename T>
    double getDegrees(const sf::Vector2<T>& vec)
    {
        // May be a vertical or null vector
        if (vec.x == 0) 
        {
            // Null vector. By convention, return 360°
            if (vec.y == 0)
                return 360;
            
            // May be pointing upwards, 90°
            else if (vec.y > 0)
                return 90;
            
            // Could only be pointing downwards, -90° = 270°
            else
                return 270;
        }

        // If not vertical or null, we may check by its tangent
        // and cuadrant
        else
        {
            // Assume right cuadrants for calculations
            double angle = radiansToDegrees(std::atan2(vec.y, vec.x));

            // Flip angle by the y-axis if on left cuadrants
            if (vec.x > 0)
            {
                // Top-left cuadrant
                if (vec.y > 0)
                    return 180 - angle;
                
                // Bottom-left cuadrant
                else
                    return 540 - angle;
            }
            else
                return angle;
        }
    }

    // If a pre-requisite comparison is satisfied with respect to a limit, return the 
    // variable advanced by a step. Otherwise, by default return the limit.
    template <typename T, typename K, typename Comparator>
    T clampAdvance(T target, const T& limit, const K& step, const Comparator& stepCondition)
    {
        if (stepCondition(target, limit))
            target += step;

        else
            target = limit;
        
        return target;
    }
}

#endif
