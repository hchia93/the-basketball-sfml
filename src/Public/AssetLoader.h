#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Defines.h"


class FAssetLoader
{
public:
	FAssetLoader() {};
	~FAssetLoader();;
	bool LoadResources();
	static SFML::Texture*	FindTexture(FAssetLoader* ContextObject, const std::string Name);
	static SFML::Font*		FindFont(FAssetLoader* ContextObject, const std::string Name);
	static SFML::Music*		FindMusic(FAssetLoader* ContextObject, const std::string Name);
	
private:
	std::map<const std::string, std::unique_ptr<SFML::Texture>> TextureLibrary;
	std::map<const std::string, std::unique_ptr<SFML::Font>>	FontLibrary;
	std::map<const std::string, std::unique_ptr<SFML::Music>>	MusicLibrary;

	bool LoadTexture(const std::string FileName);
	bool LoadFont(const std::string FileName);
	bool LoadMusic(const std::string FileName);

};

