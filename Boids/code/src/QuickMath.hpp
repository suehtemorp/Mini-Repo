#ifndef QUICKMATH_HPP
#define QUICKMATH_HPP

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

    // Get the remainder of a value between a range
    template <typename T>
    T modulus(T val, const T& min, const T& max)
    {
        while (val < min) val += max - min;
        while (val > max) val -= max - min;

        return val;
    }

    // Check if absolute difference between two values is lesser or equal
    // to a third value
    template <typename T>
    bool differenceIsSignificant(const T& a, const T& b, const T& significance)
    {return abs(a-b) <= abs(significance);}

    // Get degrees as radians
    float degreesToRadians(double degrees);

    // Get radians as degrees
    float radiansToDegrees(float radians);

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
            // atan2 considers cuadrant
            return radiansToDegrees(std::atan2(vec.y, vec.x));
    }

    // If a pre-requisite comparison is satisfied with respect to a limit, return the 
    // variable advanced by a step. Otherwise, by default return the limit.
    template <typename T, typename K, typename Comparator>
    T clampAdvance(T target, const T& limit, const K& step, const Comparator& endCondition)
    {
        if (endCondition(target, limit, step))
            target = limit;

        else
            target += step;
        
        return target;
    }
}

#endif
