#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include "Defines.h"

class FAssetLoader
{
public:
	FAssetLoader() {}
	~FAssetLoader();
	bool LoadResources();
	static sf::Texture* FindTexture(FAssetLoader* ContextObject, const std::string Name);
	static sf::Font* FindFont(FAssetLoader* ContextObject, const std::string Name);
	static sf::Music* FindMusic(FAssetLoader* ContextObject, const std::string Name);
	
private:
	std::map<const std::string, std::unique_ptr<sf::Texture>> m_TextureLibrary;
	std::map<const std::string, std::unique_ptr<sf::Font>> m_FontLibrary;
	std::map<const std::string, std::unique_ptr<sf::Music>> m_MusicLibrary;

	bool LoadTexture(const std::string fileName);
	bool LoadFont(const std::string FileName);
	bool LoadMusic(const std::string FileName);
};

