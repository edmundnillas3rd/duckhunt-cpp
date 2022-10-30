#include "DuckSpawner.h"

extern SDL_Rect gameborder;

void DuckSpawner::Init(SDL_Rect collisionBox)
{
    m_DuckCollisionbox = collisionBox;

    Reset();
}

void DuckSpawner::Update(double dt)
{
    if (m_Ducks.empty())
        Reset();

    std::vector<Duck>::iterator deadDucks;

    float newSpeed = 800.0f;


    for (Duck& duck : m_Ducks)
    {
        static float timePassed;

        timePassed += dt;

        if (timePassed > 5 && duck.currentState == Duck::States::FLYING)
        {
            duck.currentState = Sidefly();
            timePassed = 0.0f;
        } else if (timePassed > 3 && duck.currentState == Duck::States::SIDEFLY)
        {
            duck.currentState = Duck::States::FLYING;
            timePassed = 0.0f;
        }

        if (duck.Position.x > gameborder.w - duck.Collider().w || duck.Position.x < 0 )
        {
            duck.Direction.x *= -1;
        }
        else if (duck.Position.y > gameborder.h - duck.Collider().h || duck.Position.y < 0)
        {
            duck.Direction.y *= -1;
        }

        switch (duck.currentState)
        {
        case Duck::States::DEAD:
            break;
        case Duck::States::FLYING:
            duck.Transform(m_Speed * duck.Direction.x * dt * 0.75, duck.Direction.y * m_Speed * dt);
            break;
        case Duck::States::SHOT:
            duck.Transform(0, 0);
            break;
        case Duck::States::SIDEFLY:
            duck.Transform(newSpeed * duck.Direction.x * dt, duck.Direction.y * m_Speed * dt);
            break;
        case Duck::States::FALLING:
            if (duck.Direction.y < 0)
                duck.Direction.y = -duck.Direction.y;
            duck.Transform(0, duck.Direction.y * newSpeed * dt);
            break;
        }
    }

    deadDucks = remove_if(m_Ducks.begin(), m_Ducks.end(), [](const Duck& duck) {
        return duck.currentState == Duck::States::DEAD;
    });

    m_Ducks.erase(deadDucks, m_Ducks.end());
}

Duck::States DuckSpawner::Sidefly()
{
    return rand() % 6 == 0 ? Duck::States::FLYING : Duck::States::SIDEFLY;
}

void DuckSpawner::Reset()
{
    m_Speed = 300.0f;

    const int MAX_DUCKS = 2;

    for (int i = 0; i < MAX_DUCKS; i++)
    {
        int randDirX = (i + 1) % 2 == 0 ? -1 : 1;
        m_Ducks.push_back(Duck(gameborder.w / 2, gameborder.h - m_DuckCollisionbox.h, { randDirX, -1 }, m_DuckCollisionbox));
    }
}