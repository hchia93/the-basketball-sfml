#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <memory>
#include <string>
#include "Defines.h"


class FAssetLoader
{
public:
	~FAssetLoader();
	bool LoadResources();
	sf::Texture* FindTexture(const std::string fileName);
	sf::Font* FindFont(const std::string fileName);
	sf::Music* FindMusic(const std::string fileName);
	
private:
	bool LoadTexture(const std::string& filename);
	bool LoadFont(const std::string& filename);
	bool LoadMusic(const std::string& filename);

	std::map<const std::string, std::unique_ptr<sf::Texture>> m_TextureMap;
	std::map<const std::string, std::unique_ptr<sf::Font>>	m_FrontMap;
	std::map<const std::string, std::unique_ptr<sf::Music>>	m_MusicMap;

};

