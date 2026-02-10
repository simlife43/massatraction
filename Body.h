// === Body.h ===
#pragma once

#include <SDL2/SDL.h>
#include <cmath>
#include "defs.h"

struct Vector2 {
    float x, y;

    Vector2 operator-(const Vector2& other) const {
        return {x - other.x, y - other.y};
    }
    Vector2 operator+(const Vector2& other) const {
        return {x + other.x, y + other.y};
    }
    Vector2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }
    Vector2& operator+=(const Vector2& other) {
        x += other.x; y += other.y;
        return *this;
    }
    float length() const {
        return std::sqrt(x * x + y * y);
    }
    float distance(const Vector2& v) const {
        return std::sqrt((v.x-x) * (v.x-x) + (v.y-y) * (v.y-y));
    }
    Vector2 normalized() const {
        float len = length();
        return len > 0 ? Vector2{x / len, y / len} : Vector2{0, 0};
    }
};

class Body {
public:
    Vector2 position;
    Vector2 velocity;
    float mass;
    SDL_Color color;
    void updateColorFromSpeed();
    void drawBody(SDL_Renderer* renderer);
};
