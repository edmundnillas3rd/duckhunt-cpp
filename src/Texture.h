#pragma once

#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

class Texture
{
public:
    Texture() : m_Rect() {}
    ~Texture() {}

    void LoadTexture(const std::string& path);
    void LoadFont(const std::string& path, int size);
    void SetText(const std::string& text, SDL_Color color);
    void Render(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    void ClearTexture();

    SDL_Texture* m_Texture;
    TTF_Font* m_Font;

    SDL_Rect m_Rect;

    int m_Width;
    int m_Height;
};