#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>
#include "Defines.h"

class FAssetLoader
{
public:
	~FAssetLoader();
	bool LoadResources();
	static sf::Texture* FindTexture(FAssetLoader* contextObject, const std::string name);
	static sf::Font* FindFont(FAssetLoader* contextObject, const std::string name);
	static sf::Music* FindMusic(FAssetLoader* contextObject, const std::string name);
	
private:
	std::map<const std::string, std::unique_ptr<sf::Texture>> m_TextureMap;
	std::map<const std::string, std::unique_ptr<sf::Font>>	m_FontMap;
	std::map<const std::string, std::unique_ptr<sf::Music>>	m_MusicMap;

	bool LoadTexture(const std::string filename);
	bool LoadFont(const std::string filename);
	bool LoadMusic(const std::string filename);

};

