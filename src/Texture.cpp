#include "Texture.h"

extern SDL_Renderer* renderer;

void Texture::LoadTexture(const std::string& path)
{
    SDL_Texture* newTexture = nullptr;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());

    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

    m_Width = loadedSurface->w;
    m_Height = loadedSurface->h;

    m_Texture = newTexture;

    SDL_FreeSurface(loadedSurface);
}

void Texture::LoadFont(const std::string& path, int size)
{
    m_Font = TTF_OpenFont(path.c_str(), size);

}

void Texture::SetText(const std::string& text, SDL_Color color)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(m_Font, text.c_str(), color);

    if (m_Texture)
    {
		SDL_DestroyTexture(m_Texture);
        SDL_FreeSurface(textSurface);

		textSurface = TTF_RenderText_Solid(m_Font, text.c_str(), color);
		m_Texture = SDL_CreateTextureFromSurface(renderer, textSurface);

		int width, height;
		SDL_QueryTexture(m_Texture, nullptr, nullptr, &width, &height);
		m_Rect.w = width;
		m_Rect.h = height;

    }
    else
    {
        m_Texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        
        m_Width = textSurface->w;
        m_Height = textSurface->h;
    }

    
}

void Texture::Render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = { x, y, m_Width, m_Height };

    if (clip)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    else if (m_Rect.w != 0 && m_Rect.h != 0)
    {
        renderQuad.w = m_Rect.w;
        renderQuad.h = m_Rect.h;
    }

    SDL_RenderCopyEx(renderer, m_Texture, clip, &renderQuad, angle, center, flip);
}