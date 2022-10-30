#include <iostream>
#include <ctime>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "Texture.h"
#include "DuckSpawner.h"

static const uint32_t SCREEN_WIDTH  = 960;
static const uint32_t SCREEN_HEIGHT = 640;

static SDL_Window* window       = nullptr;
SDL_Renderer* renderer          = nullptr;

// Player properties
SDL_Rect cursorCollider;
const int cursorWidth   = 32;
const int cursorHeight  = 32;

// Game properties
SDL_Rect gameborder;
static double ticksCount = 0.0;
static int score = 0;
static int shots = 3;
static int duckShot = 10;
static bool leftClick = false;

// Assets
static Texture background;

static Texture duckSpriteSheet;
static Texture scoreboard;
static Texture bulletsLeftBoard;

static Mix_Chunk* duckSound = nullptr;
static Mix_Chunk* fallingDuck = nullptr;
static Mix_Chunk* shotDuck = nullptr;

// Image assets properties
static SDL_Rect backgroundRect;
static SDL_Color black { 0x00, 0x00, 0x00, 0xFF };
static SDL_RendererFlip duckSpriteFlip = SDL_FLIP_NONE;

// Animation Properties
// Flying state animation
static const int FLYING_DUCK_FRAMES = 3;
static SDL_Rect flyingDuckSpriteClips[FLYING_DUCK_FRAMES];
static int flyingDuckFrame = 0;

// Sidefly state animnation
static SDL_Rect sideflyDuckSpriteClips[FLYING_DUCK_FRAMES];

// Falling state animation
static const int FALLING_DUCK_FRAMES = 2;
static SDL_Rect fallingDuckSpriteClips[FALLING_DUCK_FRAMES];
static int fallingDuckFrame = 0;

static SDL_Rect hitDuckSpriteClip;

// Game Objects
static DuckSpawner duckSpawner;

static bool CheckCollision(SDL_Rect a, SDL_Rect b)
{
    int topA, topB;
    int leftA, leftB;
    int rightA, rightB;
    int bottomA, bottomB;

    leftA   = a.x;
    topA    = a.y;
    rightA  = a.x + a.w;
    bottomA = a.y + a.h;

    leftB   = b.x;
    topB    = b.y;
    rightB  = b.x + b.w;
    bottomB = b.y + b.h;

    if (topA >= bottomB)
    {
        return false;
    }

    if (leftA >= rightB)
    {
        return false;
    }

    if (rightA <= leftB)
    {
        return false;
    }

    if (bottomA <= topB)
    {
        return false;
    }

    return true;
}

static void GameStart()
{
    srand((unsigned) time(0));

    // Initialize Player cursor
    cursorCollider = { 0, 0, cursorWidth, cursorHeight };

    // Load Assets
    background.LoadTexture("assets/images/background.png");
    backgroundRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

    duckSpriteSheet.LoadTexture("assets/images/duck_spritesheet.png");

    scoreboard.LoadFont("assets/fonts/m29.TTF", 18);
    scoreboard.SetText("Score: " + std::to_string(score), black);

    bulletsLeftBoard.LoadFont("assets/fonts/m29.TTF", 18);
    bulletsLeftBoard.SetText("Shots: " + std::to_string(shots), black);

    duckSound = Mix_LoadWAV("assets/audio/duck.wav");
    fallingDuck = Mix_LoadWAV("assets/audio/falling_duck.wav");
    shotDuck = Mix_LoadWAV("assets/audio/shot_duck.wav");

    // Set clip rect for animation
    const int spriteWidth = duckSpriteSheet.GetWidth() / 9.5;
    flyingDuckSpriteClips[0].x = 0;
    flyingDuckSpriteClips[0].y = 0;
    flyingDuckSpriteClips[0].w = spriteWidth;
    flyingDuckSpriteClips[0].h = duckSpriteSheet.GetHeight();

    flyingDuckSpriteClips[1].x = 64;
    flyingDuckSpriteClips[1].y = 0;
    flyingDuckSpriteClips[1].w = spriteWidth;
    flyingDuckSpriteClips[1].h = duckSpriteSheet.GetHeight();

    flyingDuckSpriteClips[2].x = 128;
    flyingDuckSpriteClips[2].y = 0;
    flyingDuckSpriteClips[2].w = spriteWidth;
    flyingDuckSpriteClips[2].h = duckSpriteSheet.GetHeight();

    hitDuckSpriteClip.x = 192;
    hitDuckSpriteClip.y = 0;
    hitDuckSpriteClip.w = spriteWidth;
    hitDuckSpriteClip.h = duckSpriteSheet.GetHeight();

    fallingDuckSpriteClips[0].x = 255;
    fallingDuckSpriteClips[0].y = 0;
    fallingDuckSpriteClips[0].w = spriteWidth;
    fallingDuckSpriteClips[0].h = duckSpriteSheet.GetHeight();

    fallingDuckSpriteClips[1].x = 320;
    fallingDuckSpriteClips[1].y = 0;
    fallingDuckSpriteClips[1].w = spriteWidth;
    fallingDuckSpriteClips[1].h = duckSpriteSheet.GetHeight();


    int sideflyWidth = 80;
    sideflyDuckSpriteClips[0].x = 383;
    sideflyDuckSpriteClips[0].y = 0;
    sideflyDuckSpriteClips[0].w = sideflyWidth;
    sideflyDuckSpriteClips[0].h = duckSpriteSheet.GetHeight();

    sideflyDuckSpriteClips[1].x = 463;
    sideflyDuckSpriteClips[1].y = 0;
    sideflyDuckSpriteClips[1].w = sideflyWidth;
    sideflyDuckSpriteClips[1].h = duckSpriteSheet.GetHeight();

    sideflyDuckSpriteClips[2].x = 543;
    sideflyDuckSpriteClips[2].y = 0;
    sideflyDuckSpriteClips[2].w = sideflyWidth;
    sideflyDuckSpriteClips[2].h = duckSpriteSheet.GetHeight();

    int gameborderHeight = static_cast<int>(SCREEN_HEIGHT / 1.6);
    gameborder = { 0, 0, SCREEN_WIDTH, gameborderHeight };

    // Insert here rect for game score board
    SDL_Rect duckCollisionbox = { 0, 0, spriteWidth, duckSpriteSheet.GetHeight() };
    duckSpawner.Init(duckCollisionbox);
}

static void GameOnUpdate(double dt)
{
    scoreboard.SetText("Score: " + std::to_string(score), black);
    bulletsLeftBoard.SetText("Shots: " + std::to_string(shots), black);


    if (shots == 0)
    {
        shots = 3;
        score = 0;
    }

    if (duckSpawner.Ducks().empty())
    {
        shots = 3;
    }

    static float elapsedTime;

    elapsedTime += dt;

    flyingDuckFrame++;
    fallingDuckFrame++;

    if (flyingDuckFrame / 3 >= FLYING_DUCK_FRAMES)
    {
        flyingDuckFrame = 0;
    }

    if (flyingDuckFrame / 2 >= FALLING_DUCK_FRAMES)
    {
        fallingDuckFrame = 0;
    }

    // Update mouse cursor collider
    int x, y;
    const Uint32 mouseState = SDL_GetMouseState(&x, &y);

    cursorCollider.x = x - (cursorWidth / 2);
    cursorCollider.y = y - (cursorHeight / 2);

    duckSpawner.Update(dt);


    for (Duck& duck : duckSpawner.Ducks())
    {
        if (duck.currentState != Duck::States::SHOT || duck.currentState != Duck::States::FALLING)
        {
            duckSpriteFlip = duck.Direction.x < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

            if (duckSpriteFlip == SDL_FLIP_HORIZONTAL && duck.Direction.x > 0)
                duckSpriteFlip = SDL_FLIP_HORIZONTAL;
        }

        if (duck.currentState == Duck::States::SHOT && elapsedTime > 0.1)
        {
            duck.currentState = Duck::States::FALLING;
            elapsedTime = 0.0;
        }

        if ( leftClick && CheckCollision(cursorCollider, duck.Collider()))
        {
            duck.currentState = Duck::States::SHOT;

            score += 500;

            shots--;
            leftClick = false;
        }
        else if (leftClick && !CheckCollision(cursorCollider, duck.Collider()))
        {
            shots--;
            leftClick = false;
        }

        if (duck.currentState == Duck::States::FALLING && duck.Position.y > gameborder.h - duckSpriteSheet.GetHeight())
        {
            duck.currentState = Duck::States::DEAD;
        }
        
    }
}

static void OnClick(SDL_MouseButtonEvent& e)
{
    leftClick = true;
}

static void GameOnEvent(SDL_Event& e)
{
    switch (e.type)   
    {
    case SDL_MOUSEBUTTONDOWN:
        OnClick(e.button);
        break;
    }

    for (Duck& duck : duckSpawner.Ducks())
    {

        switch (duck.currentState)
        {
        case Duck::States::DEAD:
            continue;
        case Duck::States::FLYING:
            Mix_PlayChannel(-1, duckSound, 0);
            break;
        case Duck::States::SIDEFLY:
            break;
        case Duck::States::SHOT:
            Mix_PlayChannel(-1, shotDuck, 0);
            break;
        case Duck::States::FALLING:
            Mix_PlayChannel(-1, fallingDuck, 0);
            break;
        }
    }
}

static void GameOnRender()
{
    background.Render(0, 0, &backgroundRect);
    scoreboard.Render(50, 560);
    bulletsLeftBoard.Render( 400, 560);

    // For Debugging:
    // Draw cursor collision box relative to the mouse cursor
    // SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    // SDL_RenderDrawRect(renderer, &cursorCollider);

    // SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    // SDL_RenderDrawRect(renderer, &gameborder);


    // Render Ducks
    SDL_Rect* currentClip = nullptr;
    for (Duck& duck : duckSpawner.Ducks())
    {

        switch (duck.currentState)
        {
        case Duck::States::DEAD:
            continue;
        case Duck::States::FLYING:
            currentClip = &flyingDuckSpriteClips[flyingDuckFrame / 3];
            break;
        case Duck::States::SIDEFLY:
            currentClip = &sideflyDuckSpriteClips[flyingDuckFrame / 3];
            break;
        case Duck::States::SHOT:
            currentClip = &hitDuckSpriteClip;
            break;
        case Duck::States::FALLING:
            currentClip = &fallingDuckSpriteClips[fallingDuckFrame / 2];
            break;
        }

        duckSpriteSheet.Render( duck.Position.x, duck.Position.y, currentClip, 0.0, nullptr, duckSpriteFlip);

        // For debugging collision
        // SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        // SDL_RenderDrawRect(renderer, &duck.Collider());
    }
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    window = SDL_CreateWindow( "Duck Shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);


    SDL_Event e;

    bool running = true;

    GameStart();

    while (running)
    {
        double deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0;
        ticksCount = SDL_GetTicks();

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            GameOnEvent(e);
        }

        GameOnUpdate(deltaTime);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        GameOnRender();

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}