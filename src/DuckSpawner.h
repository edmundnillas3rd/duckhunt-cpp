#pragma once

#include <vector>
#include <algorithm>

#include <SDL.h>

#include <glm/glm.hpp>

struct Duck
{
    enum class States
    {
        DEAD = 0,
        FLYING,
        SIDEFLY,
        SHOT,
        FALLING
    };

    Duck(float x, float y, glm::ivec2 direction, SDL_Rect collisionBox)
        : Position({x, y}), Direction(direction), currentState(States::FLYING),CollisionBox(collisionBox)
    {
    }

    void Transform(float x, float y)
    {
        Position.x += x;
        Position.y += y;
    }

    SDL_Rect Collider()
    {
        return { static_cast<int>(Position.x), static_cast<int>(Position.y), CollisionBox.w, CollisionBox.h };
    }

   

    States currentState;

    glm::vec2 Position;
    glm::ivec2 Direction;
    SDL_Rect CollisionBox;
};

class DuckSpawner
{
public:
    DuckSpawner()   = default;
    ~DuckSpawner()  = default;

    void Init(SDL_Rect collision);

    void Update(double dt);
    void Reset();

    std::vector<Duck>& Ducks() { return m_Ducks; }


private:
    std::vector<Duck> m_Ducks;
    float m_Speed;
    SDL_Rect m_DuckCollisionbox;

    Duck::States Sidefly();
};