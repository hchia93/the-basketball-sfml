#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Defines.h"

class FAssetLoader
{
public:
	~FAssetLoader();
	bool LoadResources();
	static SFML::Texture* FindTexture(FAssetLoader* ContextObject, const std::string Name);
	static SFML::Font* FindFont(FAssetLoader* ContextObject, const std::string Name);
	static SFML::Music*	FindMusic(FAssetLoader* ContextObject, const std::string Name);
	
private:
	std::map<const std::string, std::unique_ptr<SFML::Texture>> m_TextureMap;
	std::map<const std::string, std::unique_ptr<SFML::Font>>	m_FontMap;
	std::map<const std::string, std::unique_ptr<SFML::Music>>	m_MusicMap;

	bool LoadTexture(const std::string filename);
	bool LoadFont(const std::string filename);
	bool LoadMusic(const std::string filename);

};

